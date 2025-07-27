// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

VulkanRenderPass::VulkanRenderPass(const RenderPassInfo& info)
    : m_Handle(VK_NULL_HANDLE)
    , m_Framebuffer(VK_NULL_HANDLE)
    , m_HasDepthStencilAttachment(false)
{
    m_HasDepthStencilAttachment = (info.DepthStencilAttachment.Texture != nullptr);
    m_Attachments = info.ColorAttachments;
    if (m_HasDepthStencilAttachment)
        m_Attachments.push_back(info.DepthStencilAttachment);

    std::vector<VkAttachmentDescription> attachmentDescriptions;
    attachmentDescriptions.reserve(m_Attachments.size());

    for (const RenderPassAttachment& attachment : m_Attachments)
    {
        VkAttachmentDescription& attachmentDescription = attachmentDescriptions.emplace_back();
        attachmentDescription.format = TextureFormatToVulkan(attachment.Texture->GetFormat());
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

        auto vulkanTexture = std::static_pointer_cast<VulkanTexture2D>(attachment.Texture);
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (vulkanTexture->IsOwnedBySwapchain())
            attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        else
            attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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

    SE_ENSURE(!m_Attachments.empty());
    const uint32 framebufferWidth = m_Attachments.front().Texture->GetSizeX();
    const uint32 framebufferHeight = m_Attachments.front().Texture->GetSizeY();

    std::vector<VkImageView> framebufferAttachments;
    framebufferAttachments.reserve(m_Attachments.size());

    for (const RenderPassAttachment& attachment : m_Attachments)
    {
        std::shared_ptr<VulkanTexture2D> attachmentTexture = std::static_pointer_cast<VulkanTexture2D>(attachment.Texture);
        framebufferAttachments.push_back(attachmentTexture->GetHandle().View);

        if (attachmentTexture->GetSizeX() != framebufferWidth || attachmentTexture->GetSizeY() != framebufferHeight)
        {
            SE_LOG_ERROR(
                "Not all provided textures have the same dimensions! (%dx%d vs %dx%d)",
                attachmentTexture->GetSizeX(), attachmentTexture->GetSizeY(),
                framebufferWidth, framebufferHeight);
            SE_ASSERT_NOT_REACHED;
            return;
        }
    }

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_Handle;
    framebufferCreateInfo.attachmentCount = (uint32)framebufferAttachments.size();
    framebufferCreateInfo.pAttachments = framebufferAttachments.data();
    framebufferCreateInfo.width = framebufferWidth;
    framebufferCreateInfo.height = framebufferHeight;
    framebufferCreateInfo.layers = 1;

    /* Create the framebuffer object. */
    SE_VULKAN_CHECK(vkCreateFramebuffer(g_VulkanDriver->GetDevice(), &framebufferCreateInfo, nullptr, &m_Framebuffer));
}

VulkanRenderPass::~VulkanRenderPass()
{
    /* TODO(Traian): Investigate if these pipelines could be in use by the time this render pass is destroyed.
     * As these pipelines are only used when this render pass is active, this should theoretically never be the case. */
    m_Pipelines.clear();

    /* Destroy the framebuffer object. */
    vkDestroyFramebuffer(g_VulkanDriver->GetDevice(), m_Framebuffer, nullptr);
    m_Framebuffer = {};

    /* Destroy the render pass object. */
    vkDestroyRenderPass(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;
}

VulkanPipeline* VulkanRenderPass::AcquireCompatiblePipeline(const GraphicsState& graphicsState)
{
    /* Check if a compatible pipeline already exists. */
    for (const auto& pipeline : m_Pipelines)
    {
        if (pipeline->IsCompatibleWithGraphicsState(graphicsState))
            return pipeline.get();
    }

    /* Create a new pipeline that matches the provided graphics state. */
    m_Pipelines.push_back(std::make_unique<VulkanPipeline>(graphicsState, m_Handle, GetColorAttachmentCount()));
    return m_Pipelines.back().get();
}

}
