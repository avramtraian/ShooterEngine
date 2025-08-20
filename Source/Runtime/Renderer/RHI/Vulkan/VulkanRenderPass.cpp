// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

VulkanRenderPass::VulkanRenderPass(const RenderPassInfo& info)
    : m_Handle(VK_NULL_HANDLE)
    , m_HasDepthStencilAttachment(false)
{
    m_HasDepthStencilAttachment = info.HasDepthStencilAttachment;
    m_Attachments = info.ColorAttachments;
    if (m_HasDepthStencilAttachment)
        m_Attachments.push_back(info.DepthStencilAttachment);

    std::vector<VkAttachmentDescription> attachmentDescriptions;
    attachmentDescriptions.reserve(m_Attachments.size());

    for (const RenderPassAttachment& attachment : m_Attachments)
    {
        VkAttachmentDescription& attachmentDescription = attachmentDescriptions.emplace_back();
        attachmentDescription.format = TextureFormatToVulkan(attachment.Format);
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

        switch (attachment.LoadOp)
        {
            case AttachmentLoadOp::Load:     attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; break;
            case AttachmentLoadOp::Clear:    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; break;
            case AttachmentLoadOp::DontCare: attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; break;
            default: SE_ASSERT_NOT_REACHED;
        }

        switch (attachment.StoreOp)
        {
            case AttachmentStoreOp::Store:    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE; break;
            case AttachmentStoreOp::DontCare: attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; break;
            default: SE_ASSERT_NOT_REACHED;
        }

        attachmentDescription.stencilLoadOp = attachmentDescription.loadOp;
        attachmentDescription.stencilStoreOp = attachmentDescription.storeOp;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = TextureLayoutToVulkan(attachment.FinalLayout);
    }

    std::vector<VkAttachmentReference> colorAttachmentReferences;
    colorAttachmentReferences.reserve(info.ColorAttachments.size());

    for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < (uint32)info.ColorAttachments.size(); ++colorAttachmentIndex)
    {
        VkAttachmentReference& attachmentReference = colorAttachmentReferences.emplace_back();
        attachmentReference.attachment = colorAttachmentIndex;
        attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkAttachmentReference depthStencilAttachmentReference = {};
    if (m_HasDepthStencilAttachment)
    {
        /* NOTE(Traian): The depth-stencil attachment is *always* the last attachment, which means that all color attachments come before it. */
        depthStencilAttachmentReference.attachment = (uint32)info.ColorAttachments.size();

        /* TODO(Traian): Use 'VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL' or 'VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL'
         * when possible instead of assuming the texture format contains both depth and stencil components. */
        depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = (uint32)colorAttachmentReferences.size();
    subpassDescription.pColorAttachments = colorAttachmentReferences.data();
    subpassDescription.pDepthStencilAttachment = m_HasDepthStencilAttachment ? &depthStencilAttachmentReference : nullptr;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = (uint32)attachmentDescriptions.size();
    renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;

    /* Create the render pass object. */
    SE_VULKAN_CHECK(vkCreateRenderPass(g_VulkanDriver->GetDevice(), &renderPassCreateInfo, nullptr, &m_Handle));
}

VulkanRenderPass::~VulkanRenderPass()
{
    m_CachedPipelines.clear();
    m_CachedFramebuffers.clear();

    /* Destroy the render pass object. */
    vkDestroyRenderPass(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;
}

RefPtr<VulkanFramebuffer> VulkanRenderPass::AcquireCompatibleFramebuffer(const RenderPassBeginInfo& beginInfo)
{
    /* Check if a compatible pipeline already exists. */
    for (auto& cachedFramebuffer : m_CachedFramebuffers)
    {
        if (cachedFramebuffer.Framebuffer->IsCompatibleWithRenderPassBeginInfo(beginInfo))
        {
            cachedFramebuffer.NumberOfFramesSinceLastUse = 0;
            return cachedFramebuffer.Framebuffer;
        }
    }

    std::vector<RefPtr<Texture2D>> framebufferTextures;
    framebufferTextures.resize(beginInfo.ColorAttachmentTextures.size());
    for (const auto& colorAttachmentTextureIt : beginInfo.ColorAttachmentTextures)
    {
        if (colorAttachmentTextureIt.first >= framebufferTextures.size())
        {
            SE_LOG_ERROR(
                "The begin info structure specifies a color attachment index that is not in the render pass specification! (AttachmetIndex: %d)",
                colorAttachmentTextureIt.first
            );
            return nullptr;
        }

        framebufferTextures[colorAttachmentTextureIt.first] = colorAttachmentTextureIt.second.Texture;
    }

    if (m_HasDepthStencilAttachment)
    {
        if (!beginInfo.DepthStencilAttachmentTexture.Texture.IsValid())
        {
            SE_LOG_ERROR("The render pass was created with a depth-stencil attachment while the render pass begin info structure doesn't have one!");
            return nullptr;
        }

        framebufferTextures.push_back(beginInfo.DepthStencilAttachmentTexture.Texture);
    }

    // TODO(Traian): Try to destroy/invalidate existing but unused framebuffers instead of creating
    // a new every time it is required. This can cause big memory leaks.
    SE_ASSERT(m_CachedFramebuffers.size() < 1024);

    // Create a new framebuffer that matches the provided render pass begin info.
    RefPtr<VulkanFramebuffer> framebuffer = CreateRef<VulkanFramebuffer>(framebufferTextures, m_Handle);
    CachedFramebuffer& cachedFramebuffer = m_CachedFramebuffers.emplace_back();
    cachedFramebuffer.Framebuffer = framebuffer;
    cachedFramebuffer.NumberOfFramesSinceLastUse = 0;

    return framebuffer;
}

RefPtr<VulkanPipeline> VulkanRenderPass::AcquireCompatiblePipeline(const GraphicsState& graphicsState)
{
    // Check if a compatible pipeline already exists.
    for (auto& cachedPipeline : m_CachedPipelines)
    {
        if (cachedPipeline.Pipeline->IsCompatibleWithGraphicsState(graphicsState))
        {
            cachedPipeline.NumberOfFramesSinceLastUse = 0;
            return cachedPipeline.Pipeline;
        }
    }

    // TODO(Traian): Try to destroy/invalidate existing but unused pipelines instead of creating
    // a new every time it is required. This can cause big memory leaks.
    SE_ASSERT(m_CachedPipelines.size() < 1024);

    // Create a new pipeline that matches the provided graphics state.
    RefPtr<VulkanPipeline> pipeline = CreateRef<VulkanPipeline>(graphicsState, m_Handle, GetColorAttachmentCount());
    CachedPipeline& cachedPipeline = m_CachedPipelines.emplace_back();
    cachedPipeline.Pipeline = pipeline;
    cachedPipeline.NumberOfFramesSinceLastUse = 0;

    return pipeline;
}

}
