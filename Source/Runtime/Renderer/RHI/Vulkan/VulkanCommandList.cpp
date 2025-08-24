// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCommandList.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanFramebuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanPipeline.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanShader.h>

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

    ReleaseObjectReferences();

    m_ParentCommandPool->RetireCommandBuffer(m_CommandBuffer);
    m_CommandBuffer = VK_NULL_HANDLE;
    m_ParentCommandPool = nullptr;
}

void VulkanCommandList::ReleaseObjectReferences()
{
    m_UsedRenderPasses.clear();
    m_UsedVertexBuffers.clear();
    m_UsedIndexBuffers.clear();

    m_UsedFramebuffers.clear();
    m_UsedPipelines.clear();
    m_UsedDescriptorSets.clear();

    // Release textures that were transitioned.
    m_TransitionedTextures.clear();

    // Release the used staging buffers.
    m_StagingBuffers.clear();
}

void VulkanCommandList::Begin()
{
    ReleaseObjectReferences();

    /* Reset draw statisticsif neccessary. */
    if (m_AccumulateStatisticsPolicy == AccumultateStatisticsPolicy::PerBeginEndCycle)
        ResetDrawStatistics();

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

        if (!(texture->GetFlags() & TEXTURE_FLAG_RENDER_TARGET))
        {
            SE_LOG_ERROR(
                "Color attachment [%d] references a texture that was not created using the 'TEXTURE_FLAG_RENDER_TARGET' flag!",
                colorAttachmentIndex);
            return false;
        }

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
    m_UsedFramebuffers.push_back(m_ActiveFramebuffer);

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

void VulkanCommandList::BindGraphicsState(const GraphicsState& graphicsState, const RefPtr<Shader>& shader)
{
    if (!shader.IsValid())
    {
        SE_LOG_ERROR("Trying to bind a graphics state with an invalid shader!");
        return;
    }

    m_ActivePipeline = m_ActiveRenderPass->AcquireCompatiblePipeline(graphicsState, shader);
    m_UsedPipelines.push_back(m_ActivePipeline);

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

bool VulkanCommandList::ValidateShaderResourcesBindPack(const RefPtr<VulkanShader>& shader, const ShaderResourcesBindPack& bindPack) const
{
    const auto& descriptorSetLayouts = shader->GetDescriptorSetLayouts();

    std::unordered_map<uint32, std::unordered_set<uint32>> missingBindingIndices;
    missingBindingIndices.reserve(descriptorSetLayouts.size());

    for (const auto& [setIndex, setLayout] : descriptorSetLayouts)
    {
        std::unordered_set<uint32>& missingBindings = missingBindingIndices[setIndex];
        missingBindings.reserve(setLayout.BindingDescriptorTypes.size());

        for (const auto& [bindingIndex, descriptorType] : setLayout.BindingDescriptorTypes)
            missingBindings.insert(bindingIndex);
    }

    for (const ShaderResourceTexture& resourceTexture : bindPack.Textures)
    {
        // Check if set exists.
        auto setLayoutIt = descriptorSetLayouts.find(resourceTexture.SetIndex);
        if (setLayoutIt == descriptorSetLayouts.end())
        {
            SE_LOG_ERROR(
                "Trying to bind a texture at a set index that doesn't exist! (Set: %d, Binding: %d)",
                resourceTexture.SetIndex, resourceTexture.BindingIndex);
            return false;
        }
        const VulkanDescriptorSetLayout& setLayout = (*setLayoutIt).second;

        // Check if binding exists.
        auto bindingIt = setLayout.BindingDescriptorTypes.find(resourceTexture.BindingIndex);
        if (bindingIt == setLayout.BindingDescriptorTypes.end())
        {
            SE_LOG_ERROR(
                "Trying to bind a texture at a binding index that doesn't exist! (Set: %d, Binding: %d)",
                resourceTexture.SetIndex, resourceTexture.BindingIndex);
            return false;
        }
        const VkDescriptorType descriptorType = (*bindingIt).second;

        // Check that the descriptor type matches the expected shader resource type.
        if (descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
        {
            SE_LOG_ERROR(
                "Trying to bind a texture at a location that represents a descriptor of another type! (Set: %d, Binding: %d)",
                resourceTexture.SetIndex, resourceTexture.BindingIndex);
            return false;
        }
        
        // Check that the provided Vulkan texture object has the 'TEXTURE_FLAG_SHADER_RESOURCE' flag.
        if (!(resourceTexture.Texture->GetFlags() & TEXTURE_FLAG_SHADER_RESOURCE))
        {
            SE_LOG_ERROR(
                "Trying to bind a texture that doesn't have the 'TEXTURE_FLAG_SHADER_RESOURCE' flag! (Set: %d, Binding: %d)",
                resourceTexture.SetIndex, resourceTexture.BindingIndex);
            return false;
        }

        // Remove the binding index from the missing list.
        missingBindingIndices.at(resourceTexture.SetIndex).erase(resourceTexture.BindingIndex);
    }

    for (const ShaderResourceUniformBuffer& resourceUniformBuffer : bindPack.UniformBuffers)
    {
        // Check if set exists.
        auto setLayoutIt = descriptorSetLayouts.find(resourceUniformBuffer.SetIndex);
        if (setLayoutIt == descriptorSetLayouts.end())
        {
            SE_LOG_ERROR(
                "Trying to bind a uniform buffer at a set index that doesn't exist! (Set: %d, Binding: %d)",
                resourceUniformBuffer.SetIndex, resourceUniformBuffer.BindingIndex);
            return false;
        }
        const VulkanDescriptorSetLayout& setLayout = (*setLayoutIt).second;

        // Check if binding exists.
        auto bindingIt = setLayout.BindingDescriptorTypes.find(resourceUniformBuffer.BindingIndex);
        if (bindingIt == setLayout.BindingDescriptorTypes.end())
        {
            SE_LOG_ERROR(
                "Trying to bind a uniform buffer at a binding index that doesn't exist! (Set: %d, Binding: %d)",
                resourceUniformBuffer.SetIndex, resourceUniformBuffer.BindingIndex);
            return false;
        }
        const VkDescriptorType descriptorType = (*bindingIt).second;

        // Check that the descriptor type matches the expected shader resource type.
        if (descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        {
            SE_LOG_ERROR(
                "Trying to bind a uniform buffer at a location that represents a descriptor of another type! (Set: %d, Binding: %d)",
                resourceUniformBuffer.SetIndex, resourceUniformBuffer.BindingIndex);
            return false;
        }

        // Remove the binding index from the missing list.
        missingBindingIndices.at(resourceUniformBuffer.SetIndex).erase(resourceUniformBuffer.BindingIndex);
    }

    // Check if there are any missing binding indices.
    for (const auto& [setIndex, missingBindings] : missingBindingIndices)
    {
        if (!missingBindings.empty())
        {
            SE_LOG_ERROR("The set with index '%d' has the following missing binding indices:", setIndex);
            for (uint32 bindingIndex : missingBindings)
            {
                SE_LOG_ERROR(
                    "  - [%d] (DescriptorType: %d)",
                    bindingIndex, descriptorSetLayouts.at(setIndex).BindingDescriptorTypes.at(bindingIndex));
            }
            return false;
        }
    }
    
    return true;
}

void VulkanCommandList::BindShaderResources(const ShaderResourcesBindPack& bindPack)
{
    RefPtr<VulkanShader> activeShader = m_ActivePipeline->GetShader();
    if (!ValidateShaderResourcesBindPack(activeShader, bindPack))
        return;

    for (const ShaderResourceTexture& shaderResourceTexture : bindPack.Textures)
    {
        RefPtr<VulkanStorageTexture2D> vulkanTexture = shaderResourceTexture.Texture.As<VulkanStorageTexture2D>();
        if (vulkanTexture->IsPendingUploadData())
        {
            // Create and fill the staging buffer.
            VulkanBuffer& stagingBuffer = CreateStagingBuffer(vulkanTexture->GetPendingTextureData().ByteCount());
            void* mappedStagingBufferData = stagingBuffer.Map(0, vulkanTexture->GetPendingTextureData().ByteCount());
            MemoryCopy(mappedStagingBufferData, vulkanTexture->GetPendingTextureData().Bytes(), vulkanTexture->GetPendingTextureData().ByteCount());
            stagingBuffer.Unmap();

            // Record the texture transisions and copy commands.
            vulkanTexture->GenerateUploadDataCommands(AdoptRef(this), stagingBuffer);
        }
    }

    std::vector<VulkanDescriptorSet*> descriptorSets = activeShader->GetDescriptorSetManager().AcquireDescriptorSets(bindPack);

    std::vector<VkDescriptorSet> descriptorSetHandles;
    descriptorSetHandles.reserve(descriptorSets.size());
    m_UsedDescriptorSets.reserve(m_UsedDescriptorSets.size() + descriptorSets.size());

    for (VulkanDescriptorSet* descriptorSet : descriptorSets)
    {
        m_UsedDescriptorSets.push_back(descriptorSet);
        descriptorSetHandles.push_back(descriptorSet->GetHandle());
    }

    vkCmdBindDescriptorSets(
        m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        activeShader->GetPipelineLayout(), 0,
        (uint32)descriptorSetHandles.size(), descriptorSetHandles.data(), // Descriptor sets.
        0, nullptr                                                        // Dynamic offses.
    );
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
    if (!m_ActiveRenderPass.IsValid() || !m_ActiveFramebuffer.IsValid())
    {
        SE_LOG_ERROR("Trying to call DrawIndexed without a render pass being active!");
        return;
    }
    if (!m_ActivePipeline.IsValid())
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

inline VkPipelineStageFlags PipelineStageBitsToVulkan(PipelineStageBits stages)
{
    VkPipelineStageFlags pipelineStageFlags = 0;
    if (stages & PIPELINE_STAGE_TOP_OF_PIPE_BIT)             { pipelineStageFlags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; }
    if (stages & PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT) { pipelineStageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; }
    if (stages & PIPELINE_STAGE_TRANSFER_BIT)                { pipelineStageFlags |= VK_PIPELINE_STAGE_TRANSFER_BIT; }
    if (stages & PIPELINE_STAGE_VERTEX_SHADER_BIT)           { pipelineStageFlags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT; }
    if (stages & PIPELINE_STAGE_FRAGMENT_SHADER_BIT)         { pipelineStageFlags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; }

    SE_ASSERT(pipelineStageFlags != 0);
    return pipelineStageFlags;
}

inline VkAccessFlags AccessFlagsBitsToVulkan(AccessFlagsBits accessFlags)
{
    VkAccessFlags vulkanAccessFlags = VK_ACCESS_NONE;
    if (accessFlags & ACCESS_FLAG_VERTEX_ATTRIBUTE_READ_BIT)          { vulkanAccessFlags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT; }
    if (accessFlags & ACCESS_FLAG_UNIFORM_READ_BIT)                   { vulkanAccessFlags |= VK_ACCESS_UNIFORM_READ_BIT; }
    if (accessFlags & ACCESS_FLAG_INPUT_ATTACHMENT_READ_BIT)          { vulkanAccessFlags |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; }
    if (accessFlags & ACCESS_FLAG_SHADER_READ_BIT)                    { vulkanAccessFlags |= VK_ACCESS_SHADER_READ_BIT; }
    if (accessFlags & ACCESS_FLAG_SHADER_WRITE_BIT)                   { vulkanAccessFlags |= VK_ACCESS_SHADER_WRITE_BIT; }
    if (accessFlags & ACCESS_FLAG_COLOR_ATTACHMENT_READ_BIT)          { vulkanAccessFlags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT; }
    if (accessFlags & ACCESS_FLAG_COLOR_ATTACHMENT_WRITE_BIT)         { vulkanAccessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; }
    if (accessFlags & ACCESS_FLAG_DEPTH_STENCIL_ATTACHMENT_READ_BIT)  { vulkanAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT; }
    if (accessFlags & ACCESS_FLAG_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT) { vulkanAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; }
    if (accessFlags & ACCESS_FLAG_TRANSFER_READ_BIT)                  { vulkanAccessFlags |= VK_ACCESS_TRANSFER_READ_BIT; }
    if (accessFlags & ACCESS_FLAG_TRANSFER_WRITE_BIT)                 { vulkanAccessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT; }

    return vulkanAccessFlags;
}

void VulkanCommandList::TransitionTexture(const TransitionTextureInfo& info)
{
    auto vulkanTexture = info.Texture.As<VulkanTexture2D>();
    m_TransitionedTextures.push_back(vulkanTexture);

    const VkImageAspectFlags imageAspect = IsTextureDepthFormat(vulkanTexture->GetFormat())
        ? VK_IMAGE_ASPECT_DEPTH_BIT
        : VK_IMAGE_ASPECT_COLOR_BIT;

    VkImageMemoryBarrier imageBarrier = {};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.srcAccessMask = AccessFlagsBitsToVulkan(info.SrcAccessFlags);
    imageBarrier.dstAccessMask = AccessFlagsBitsToVulkan(info.DstAccessFlags);
    imageBarrier.oldLayout = TextureLayoutToVulkan(info.OldLayout);
    imageBarrier.newLayout = TextureLayoutToVulkan(info.NewLayout);
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = vulkanTexture->GetHandle().Image;
    imageBarrier.subresourceRange.aspectMask = imageAspect;
    imageBarrier.subresourceRange.baseMipLevel = 0;
    imageBarrier.subresourceRange.levelCount = 1;
    imageBarrier.subresourceRange.baseArrayLayer = 0;
    imageBarrier.subresourceRange.layerCount = 1;

    const VkPipelineStageFlags srcStage = PipelineStageBitsToVulkan(info.SrcPipelineStages);
    const VkPipelineStageFlags dstStage = PipelineStageBitsToVulkan(info.DstPipelineStages);

    vkCmdPipelineBarrier(
        m_CommandBuffer, srcStage, dstStage, 0,
        0, nullptr,      // Memory barries.
        0, nullptr,      // Buffer memory barries.
        1, &imageBarrier // Image memory barries.
    );
}

void VulkanCommandList::CopyBufferToImage(const RefPtr<VulkanTexture2D>& dstTexture, VkBuffer srcBuffer)
{
    const VkImageAspectFlags imageAspect = IsTextureDepthFormat(dstTexture->GetFormat())
        ? VK_IMAGE_ASPECT_DEPTH_BIT
        : VK_IMAGE_ASPECT_COLOR_BIT;

    VkBufferImageCopy imageCopyRegion = {};
    imageCopyRegion.bufferOffset = 0;
    imageCopyRegion.bufferRowLength = 0;
    imageCopyRegion.bufferImageHeight = 0;
    imageCopyRegion.bufferImageHeight = 0;
    imageCopyRegion.imageSubresource.aspectMask = imageAspect;
    imageCopyRegion.imageSubresource.mipLevel = 0;
    imageCopyRegion.imageSubresource.baseArrayLayer = 0;
    imageCopyRegion.imageSubresource.layerCount = 1;
    imageCopyRegion.imageOffset.x = 0;
    imageCopyRegion.imageOffset.y = 0;
    imageCopyRegion.imageOffset.z = 0;
    imageCopyRegion.imageExtent.width = dstTexture->GetSizeX();
    imageCopyRegion.imageExtent.height = dstTexture->GetSizeY();
    imageCopyRegion.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(
        m_CommandBuffer,
        srcBuffer,
       dstTexture->GetHandle().Image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &imageCopyRegion
    );
}

VulkanBuffer& VulkanCommandList::CreateStagingBuffer(usize bufferSize)
{
    auto stagingBuffer = std::make_unique<VulkanBuffer>();
    stagingBuffer->Invalidate(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_StagingBuffers.push_back(std::move(stagingBuffer));
    return *m_StagingBuffers.back().get();
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
