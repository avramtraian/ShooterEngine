// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCommandList.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanFramebuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanPipeline.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

VulkanCommandList::VulkanCommandList(const CommandListInfo& info)
    : m_CommandBuffer(VK_NULL_HANDLE)
    , m_Family(info.Family)
    , m_ActivePipeline(nullptr)
    , m_IsVertexBufferBound(false)
    , m_IsIndexBufferBound(false)
    , m_AccumulateStatisticsPolicy(AccumultateStatisticsPolicy::PerBeginEndCycle)
{
    if (m_Family == CommandListFamily::Unknown)
    {
        SE_LOG_ERROR("Unspecified [Vulkan] command list type!");
        SE_ASSERT_NOT_REACHED;
        return;
    }

    m_ParentCommandPool = g_VulkanDriver->GetCommandPool(g_VulkanDriver->GetQueueFamilyIndices().Graphics);
    m_CommandBuffer = m_ParentCommandPool->AcquireCommandBuffer();
}

VulkanCommandList::~VulkanCommandList()
{
    VkQueue submisionQueue = VK_NULL_HANDLE;
    switch (m_Family)
    {
        case CommandListFamily::Graphics: submisionQueue = g_VulkanDriver->GetGraphicsQueue(); break;
        case CommandListFamily::Transfer: submisionQueue = g_VulkanDriver->GetTransferQueue(); break;
        case CommandListFamily::Compute:  submisionQueue = g_VulkanDriver->GetComputeQueue(); break;
        default: SE_ASSERT_NOT_REACHED;
    }

    /* NOTE(Traian): Currently, the fence allocation architecture doesn't allow for reference tracking and
     * there is no way to signal multiple fences when the 'vkQueueSubmit' function finishes. Unfortunately,
     * this means that we have to wait for the entire queue to finish before being able to safely retire
     * the command buffer. */
    SE_VULKAN_CHECK(vkQueueWaitIdle(submisionQueue));

    m_ParentCommandPool->RetireCommandBuffer(m_CommandBuffer);
    m_CommandBuffer = VK_NULL_HANDLE;
    m_ParentCommandPool = nullptr;
}

void VulkanCommandList::Begin()
{
    /* Reset draw statisticsif neccessary. */
    if (m_AccumulateStatisticsPolicy == AccumultateStatisticsPolicy::PerBeginEndCycle)
        ResetDrawStatistics();

    /* Release all resource references previously held by the command list. */
    m_UsedRenderPasses.clear();
    m_UsedVertexBuffers.clear();
    m_UsedIndexBuffers.clear();

    /* Reset the command buffer before reusing it. */
    SE_VULKAN_CHECK(vkResetCommandBuffer(m_CommandBuffer, 0));

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    SE_VULKAN_CHECK(vkBeginCommandBuffer(m_CommandBuffer, &commandBufferBeginInfo));
}

void VulkanCommandList::End()
{
    SE_VULKAN_CHECK(vkEndCommandBuffer(m_CommandBuffer));

    m_IsVertexBufferBound = false;
    m_IsIndexBufferBound = false;
}

bool VulkanCommandList::ValidateRenderPass(const RefPtr<VulkanRenderPass>& renderPass, const RenderPassBeginInfo& beginInfo) const
{
    if (renderPass->GetAttachmentCount() == 0)
    {
        SE_LOG_ERROR("Trying to begin a render pass that has no attachments!");
        SE_LOG_ERROR("A render pass with no attachments has been encountered. This resource should not be possible to be created!");
        return false;
    }

    if (beginInfo.ColorAttachmentTextures.size() != renderPass->GetColorAttachmentCount())
    {
        SE_LOG_ERROR(
            "The number of color attachments specified in the 'RenderPassBeginInfo' structure doesn't match the render pass specification! (%d vs %s)",
            beginInfo.ColorAttachmentTextures.size(), renderPass->GetColorAttachmentCount());
        return false;
    }

    if (renderPass->HasDepthStencilAttachment() && !beginInfo.DepthStencilAttachmentTexture.Texture.IsValid())
    {
        SE_LOG_ERROR("The render pass was created with a depth-stencil attachment but the 'RenderPassBeginInfo' doesn't have one!");
        return false;
    }

    if (!renderPass->HasDepthStencilAttachment() && beginInfo.DepthStencilAttachmentTexture.Texture.IsValid())
    {
        SE_LOG_WARN("The render pass was created without a depth-stencil attachment but the 'RenderPassBeginInfo' has one!");
        return false;
    }
    
    uint32 framebufferWidth = 0;
    uint32 framebufferHeight = 0;

    if (renderPass->HasDepthStencilAttachment())
    {
        framebufferWidth = beginInfo.DepthStencilAttachmentTexture.Texture->GetSizeX();
        framebufferHeight = beginInfo.DepthStencilAttachmentTexture.Texture->GetSizeY();
    }
    else
    {
        framebufferWidth = (*beginInfo.ColorAttachmentTextures.begin()).second.Texture->GetSizeX();
        framebufferHeight = (*beginInfo.ColorAttachmentTextures.begin()).second.Texture->GetSizeY();
    }

    int32 maxColorAttachmentIndex = -1;
    for (const auto& colorAttachmentTextureIt : beginInfo.ColorAttachmentTextures)
    {
        const uint32 colorAttachmentIndex = colorAttachmentTextureIt.first;
        if ((int32)colorAttachmentIndex > maxColorAttachmentIndex)
            maxColorAttachmentIndex = colorAttachmentIndex;
        const auto& texture = colorAttachmentTextureIt.second.Texture;

        if (texture->GetSizeX() != framebufferWidth || texture->GetSizeY() != framebufferHeight)
        {
            SE_LOG_ERROR(
                "Not all textures provided to the render pass begin function have the same dimensions! (%dx%d vs %dx%d)",
                texture->GetSizeX(), texture->GetSizeY(),
                framebufferWidth, framebufferHeight);
            return false;
        }
    }

    return true;
}

void VulkanCommandList::BeginRenderPass(const RefPtr<RenderPass>& renderPass, const RenderPassBeginInfo& beginInfo)
{
    auto vulkanRenderPass = renderPass.As<VulkanRenderPass>();
    if (!ValidateRenderPass(vulkanRenderPass, beginInfo))
        return;

    m_ActiveRenderPass = vulkanRenderPass;
    m_UsedRenderPasses.push_back(m_ActiveRenderPass);

    std::vector<VkClearValue> clearValues;
    clearValues.reserve(m_ActiveRenderPass->GetAttachmentCount());
    for (uint32 attachmentIndex = 0; attachmentIndex < m_ActiveRenderPass->GetAttachmentCount(); ++attachmentIndex)
    {
        const RenderPassAttachment& attachment = m_ActiveRenderPass->GetAttachment(attachmentIndex);
        const RenderPassAttachmentTexture& attachmentTexture =
            (attachmentIndex < m_ActiveRenderPass->GetColorAttachmentCount())
                ? beginInfo.ColorAttachmentTextures.at(attachmentIndex)
                : beginInfo.DepthStencilAttachmentTexture;
        SE_ASSERT(attachmentTexture.Texture.IsValid());

        VkClearValue& clearValue = clearValues.emplace_back();

        /* NOTE(Traian): Since both our clear value 'RenderPassAttachmentClearValue' structure and the Vulkan 'VkClearValue' structure
         * have the same memory layout this "assignment" is sufficient to copy all data. */
        clearValue.color.float32[0] = attachmentTexture.ClearValue.Float32[0];
        clearValue.color.float32[1] = attachmentTexture.ClearValue.Float32[1];
        clearValue.color.float32[2] = attachmentTexture.ClearValue.Float32[2];
        clearValue.color.float32[3] = attachmentTexture.ClearValue.Float32[3];
    }

    m_ActiveFramebuffer = m_ActiveRenderPass->AcquireCompatibleFramebuffer(beginInfo);

    VkRenderPassBeginInfo renderPassBeginInfo = {}; 
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_ActiveRenderPass->GetHandle();
    renderPassBeginInfo.framebuffer = m_ActiveFramebuffer->GetHandle();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = m_ActiveFramebuffer->GetSizeX();
    renderPassBeginInfo.renderArea.extent.height = m_ActiveFramebuffer->GetSizeY();
    renderPassBeginInfo.clearValueCount = (uint32)clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandList::EndRenderPass()
{
    vkCmdEndRenderPass(m_CommandBuffer);

    m_ActiveRenderPass = nullptr;
    m_ActiveFramebuffer = nullptr;
    m_ActivePipeline = nullptr;
}

void VulkanCommandList::BindGraphicsState(const GraphicsState& graphicsState)
{
    m_ActivePipeline = m_ActiveRenderPass->AcquireCompatiblePipeline(graphicsState);
    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ActivePipeline->GetHandle());

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)m_ActiveFramebuffer->GetSizeX();
    viewport.height = (float)m_ActiveFramebuffer->GetSizeY();
    viewport.minDepth = 0.0F;
    viewport.maxDepth = 1.0F;

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = m_ActiveFramebuffer->GetSizeX();
    scissor.extent.height = m_ActiveFramebuffer->GetSizeY();

    vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
}

void VulkanCommandList::BindVertexBuffer(const RefPtr<VertexBuffer>& vertexBuffer)
{
    auto vulkanVertexBuffer = vertexBuffer.As<VulkanVertexBuffer>();
    VkBuffer bufferHandle = vulkanVertexBuffer->GetHandle();
    VkDeviceSize bufferOffset = 0;

    /* Submit command to the command buffer. */
    vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &bufferHandle, &bufferOffset);
    m_UsedVertexBuffers.push_back(vulkanVertexBuffer);
    m_IsVertexBufferBound = true;
}

void VulkanCommandList::BindIndexBuffer(const RefPtr<IndexBuffer>& indexBuffer)
{
    auto vulkanIndexBuffer = indexBuffer.As<VulkanIndexBuffer>();
    VkBuffer bufferHandle = vulkanIndexBuffer->GetHandle();

    VkIndexType indexType = VK_INDEX_TYPE_UINT16;
    switch (vulkanIndexBuffer->GetDataType())
    {
        case IndexBufferDataType::UInt16: indexType = VK_INDEX_TYPE_UINT16; break;
        case IndexBufferDataType::UInt32: indexType = VK_INDEX_TYPE_UINT32; break;
        default: SE_ASSERT_NOT_REACHED;
    }

    /* Submit command to the command buffer. */
    vkCmdBindIndexBuffer(m_CommandBuffer, vulkanIndexBuffer->GetHandle(), 0, indexType);
    m_UsedIndexBuffers.push_back(vulkanIndexBuffer);
    m_IsIndexBufferBound = true;
}

void VulkanCommandList::DrawIndexed(uint32 firstIndex, uint32 indexCount)
{
    /* Validate command list state. */
    if (!m_ActiveRenderPass.IsValid())
    {
        SE_LOG_ERROR("Trying to call DrawIndexed without a render pass being active!");
        return;
    }
    if (!m_ActivePipeline)
    {
        SE_LOG_ERROR("Trying to call DrawIndexed without a graphics state being bound!");
        return;
    }
    if (!m_IsVertexBufferBound)
    {
        SE_LOG_ERROR("Trying to call DrawIndexed without a vertex buffer being bound!");
        return;
    }
    if (!m_IsIndexBufferBound)
    {
        SE_LOG_ERROR("Trying to call DrawIndexed without an index buffer being bound!");
        return;
    }

    /* Submit command to the command buffer. */
    vkCmdDrawIndexed(m_CommandBuffer, indexCount, 1, firstIndex, 0, 0);

    /* Update draw statistics. */
    m_DrawStatistics.DrawCalls++;
    m_DrawStatistics.Vertices += indexCount;

    switch (m_ActivePipeline->GetGraphicsState().Topology)
    {
        case GraphicsTopology::TriangleList: m_DrawStatistics.Triangles += (indexCount / 3); break;

        default: SE_ASSERT_NOT_REACHED; /* TODO(Traian): Implement all primitive topologies! */
    }
}

void VulkanCommandList::ResetDrawStatistics()
{
    m_DrawStatistics = {};
}

void VulkanCommandList::SetAccumulateStatisticsPolicy(AccumultateStatisticsPolicy policy)
{
    m_AccumulateStatisticsPolicy = policy;
}

}
