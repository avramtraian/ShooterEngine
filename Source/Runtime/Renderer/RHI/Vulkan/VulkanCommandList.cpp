// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCommandList.h>
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

void VulkanCommandList::BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass)
{
    m_ActiveRenderPass = std::static_pointer_cast<VulkanRenderPass>(renderPass);
    m_UsedRenderPasses.push_back(m_ActiveRenderPass);

    std::vector<VkClearValue> clearValues;
    clearValues.reserve(m_ActiveRenderPass->GetAttachmentCount());
    for (uint32 attachmentIndex = 0; attachmentIndex < m_ActiveRenderPass->GetAttachmentCount(); ++attachmentIndex)
    {
        const RenderPassAttachment& attachment = m_ActiveRenderPass->GetAttachment(attachmentIndex);
        VkClearValue& clearValue = clearValues.emplace_back();

        /* NOTE(Traian): Since both our clear value 'RenderPassAttachmentClearValue' structure and the Vulkan 'VkClearValue' structure
         * have the same memory layout this "assignment" is sufficient to copy all data. */
        clearValue.color.float32[0] = attachment.ClearValue.Float32[0];
        clearValue.color.float32[1] = attachment.ClearValue.Float32[1];
        clearValue.color.float32[2] = attachment.ClearValue.Float32[2];
        clearValue.color.float32[3] = attachment.ClearValue.Float32[3];
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {}; 
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = m_ActiveRenderPass->GetHandle();
    renderPassBeginInfo.framebuffer = m_ActiveRenderPass->GetFramebuffer();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = m_ActiveRenderPass->GetFramebufferSizeX();
    renderPassBeginInfo.renderArea.extent.height = m_ActiveRenderPass->GetFramebufferSizeY();
    renderPassBeginInfo.clearValueCount = (uint32)clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandList::EndRenderPass()
{
    vkCmdEndRenderPass(m_CommandBuffer);
    m_ActiveRenderPass = nullptr;
    m_ActivePipeline = nullptr;
}

void VulkanCommandList::BindGraphicsState(const GraphicsState& graphicsState)
{
    m_ActivePipeline = m_ActiveRenderPass->AcquireCompatiblePipeline(graphicsState);
    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ActivePipeline->GetHandle());

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)m_ActiveRenderPass->GetFramebufferSizeX();
    viewport.height = (float)m_ActiveRenderPass->GetFramebufferSizeY();
    viewport.minDepth = 0.0F;
    viewport.maxDepth = 1.0F;

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = m_ActiveRenderPass->GetFramebufferSizeX();
    scissor.extent.height = m_ActiveRenderPass->GetFramebufferSizeY();

    vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
}

void VulkanCommandList::BindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    auto vulkanVertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer);
    VkBuffer bufferHandle = vulkanVertexBuffer->GetHandle();
    VkDeviceSize bufferOffset = 0;

    /* Submit command to the command buffer. */
    vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &bufferHandle, &bufferOffset);
    m_UsedVertexBuffers.push_back(vulkanVertexBuffer);
    m_IsVertexBufferBound = true;
}

void VulkanCommandList::BindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    auto vulkanIndexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer);
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
    if (!m_ActiveRenderPass)
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
