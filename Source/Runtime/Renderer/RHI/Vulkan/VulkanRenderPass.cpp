// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

VulkanRenderPassLayout VulkanRenderPass::GetLayoutFromInfo(const RenderPassInfo& info)
{
    VulkanRenderPassLayout layout = {};
    return layout;
}

static bool CompareAttachments(const VulkanRenderPassAttachment& attachmentA, const VulkanRenderPassAttachment& attachmentB)
{
    const bool result =
        (attachmentA.Format == attachmentB.Format) &&
        (attachmentA.LoadOp == attachmentB.LoadOp) &&
        (attachmentA.StoreOp == attachmentB.StoreOp) &&
        (attachmentA.InitialLayout == attachmentB.InitialLayout) &&
        (attachmentA.FinalLayout == attachmentB.FinalLayout);
    return result;
}

bool VulkanRenderPass::CheckIfLayoutsAreCompatible(const VulkanRenderPassLayout& layoutA, const VulkanRenderPassLayout& layoutB)
{
    if (layoutA.ColorAttachments.size() != layoutB.ColorAttachments.size())
        return false;

    if ((layoutA.HasDepthStencilAttachment && !layoutB.HasDepthStencilAttachment) ||
        (!layoutA.HasDepthStencilAttachment && layoutB.HasDepthStencilAttachment))
    {
        return false;
    }

    for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < (uint32)layoutA.ColorAttachments.size(); ++colorAttachmentIndex)
    {
        const VulkanRenderPassAttachment& attachmentA = layoutA.ColorAttachments[colorAttachmentIndex];
        const VulkanRenderPassAttachment& attachmentB = layoutB.ColorAttachments[colorAttachmentIndex];
        if (!CompareAttachments(attachmentA, attachmentB))
            return false;
    }

    if (layoutA.HasDepthStencilAttachment)
    {
        if (!CompareAttachments(layoutA.DepthStencilAttachment, layoutB.DepthStencilAttachment))
            return false;
    }

    return true;
}

VulkanRenderPass::VulkanRenderPass(const VulkanRenderPassLayout& layout)
    : m_Handle(VK_NULL_HANDLE)
    , m_Layout(layout)
{
    /* NOTE(Traian): The order in wich framebuffer attachments appear in the 'VkFramebufferCreateInfo.pAttachments' must match the order
     * in which they appear in the 'VkRenderPassCreateInfo.pAttachments'. In order to ensure this contraint, the color attachments are
     * always enumerated first, and the depth-stencil attachment (if exists) is put at the end. */
    std::vector<VulkanRenderPassAttachment> renderPassAttachments = m_Layout.ColorAttachments;
    if (m_Layout.HasDepthStencilAttachment)
        renderPassAttachments.push_back(m_Layout.DepthStencilAttachment);

    std::vector<VkAttachmentDescription> attachmentDescriptions;
    attachmentDescriptions.reserve(renderPassAttachments.size());
    for (const VulkanRenderPassAttachment& attachment : renderPassAttachments)
    {
        VkAttachmentDescription& attachmentDescription = attachmentDescriptions.emplace_back();
        attachmentDescription.format = attachment.Format;
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.loadOp = attachment.LoadOp;
        attachmentDescription.storeOp = attachment.StoreOp;
        attachmentDescription.initialLayout = attachment.InitialLayout;
        attachmentDescription.finalLayout = attachment.FinalLayout;
    }

    std::vector<VkAttachmentReference> subpassColorAttachments;
    subpassColorAttachments.reserve(m_Layout.ColorAttachments.size());

    for (uint32 attachmentIndex = 0; attachmentIndex < (uint32)m_Layout.ColorAttachments.size(); ++attachmentIndex)
    {
        VkAttachmentReference& attachmentReference = subpassColorAttachments.emplace_back();
        attachmentReference.attachment = attachmentIndex;
        attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkAttachmentReference subpassDepthStencilAttachment = {};
    if (m_Layout.HasDepthStencilAttachment)
    {
        subpassDepthStencilAttachment.attachment = (uint32)(renderPassAttachments.size() - 1);
        /* TODO(Traian): Use 'VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL' or 'VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL'
         * when possible instead of assuming the texture format contains both depth and stencil components. */
        subpassDepthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = (uint32)subpassColorAttachments.size();
    subpassDescription.pColorAttachments = subpassColorAttachments.data();
    subpassDescription.pDepthStencilAttachment = m_Layout.HasDepthStencilAttachment ? &subpassDepthStencilAttachment : nullptr;

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
    /* Destroy the render pass object. */
    vkDestroyRenderPass(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;
    m_Layout = {};
}

}
