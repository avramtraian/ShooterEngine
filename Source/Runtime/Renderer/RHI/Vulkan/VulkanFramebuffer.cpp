// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/CommandList.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanFramebuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{


VulkanFramebufferInfo VulkanFramebuffer::GetInfoFromRenderPass(VkRenderPass renderPassHandle, const RenderPassInfo& renderPassInfo)
{
    const bool hasDepthStencilAttachment = (renderPassInfo.DepthStencilAttachment.Texture != nullptr);

    /* NOTE(Traian): The order in wich framebuffer attachments appear in the 'VkFramebufferCreateInfo.pAttachments' must match the order
     * in which they appear in the 'VkRenderPassCreateInfo.pAttachments'. In order to ensure this contraint, the color attachments are
     * always enumerated first, and the depth-stencil attachment (if exists) is put at the end. */
    std::vector<RenderPassAttachment> renderPassAttachments = renderPassInfo.ColorAttachments;
    if (hasDepthStencilAttachment)
        renderPassAttachments.push_back(renderPassInfo.DepthStencilAttachment);

    if (renderPassAttachments.empty())
    {
        SE_LOG_ERROR("The provided render pass has no depth-stencil attachment and no color attachments!");
        return {};
    }

    const uint32 framebufferWidth = renderPassAttachments[0].Texture->GetSizeX();
    const uint32 framebufferHeight = renderPassAttachments[0].Texture->GetSizeY();

    /* Ensure that all provided textures have the same dimensions. */
    for (uint32 attachmentIndex = 0; attachmentIndex < renderPassAttachments.size(); ++attachmentIndex)
    {
        const RenderPassAttachment& attachment = renderPassAttachments[attachmentIndex];
        if (attachment.Texture->GetSizeX() != framebufferWidth || attachment.Texture->GetSizeY() != framebufferHeight)
        {
            SE_LOG_ERROR("Not all textures provided to the [Vulkan] framebuffer creation have the same dimensions!");
            return {};
        }
    }

    VulkanFramebufferInfo framebufferInfo = {};
    framebufferInfo.Width = framebufferWidth;
    framebufferInfo.Height = framebufferHeight;
    framebufferInfo.RenderPass = renderPassHandle;
    
    framebufferInfo.Attachments.reserve(renderPassAttachments.size());
    for (const RenderPassAttachment& attachment : renderPassAttachments)
        framebufferInfo.Attachments.push_back(std::static_pointer_cast<VulkanTexture2D>(attachment.Texture));

    return framebufferInfo;
}

bool VulkanFramebuffer::CheckIfFramebuffersAreCompatible(const VulkanFramebufferInfo& infoA, const VulkanFramebufferInfo& infoB)
{
    /* Check dimensions. */
    if (infoA.Width != infoB.Width || infoA.Height != infoB.Height)
        return false;

    /* If both framebuffers use the same render pass then they are obviously compatible. */
    if (infoA.RenderPass == infoB.RenderPass)
        return true;
    
    /* Check if they have the same number of attachments. */
    if (infoA.Attachments.size() != infoB.Attachments.size())
        return false;

    for (uint32 attachmentIndex = 0; attachmentIndex < (uint32)infoA.Attachments.size(); ++attachmentIndex)
    {
        const auto& attachmentTextureA = infoA.Attachments[attachmentIndex];
        const auto& attachmentTextureB = infoB.Attachments[attachmentIndex];

        if ((attachmentTextureA->GetHandle().Image != attachmentTextureB->GetHandle().Image) ||
            (attachmentTextureA->GetHandle().View != attachmentTextureB->GetHandle().View))
        {
            return false;
        }
    }
    
    /* The framebuffers are compatible. */
    return true;
}

VulkanFramebuffer::VulkanFramebuffer(const VulkanFramebufferInfo& info)
    : m_Handle(VK_NULL_HANDLE)
    , m_Info(info)
{
    std::vector<VkImageView> framebufferAttachments;
    framebufferAttachments.reserve(m_Info.Attachments.size());
    for (const auto& attachmentTexture : m_Info.Attachments)
        framebufferAttachments.push_back(attachmentTexture->GetHandle().View);

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_Info.RenderPass;
    framebufferCreateInfo.attachmentCount = (uint32)framebufferAttachments.size();
    framebufferCreateInfo.pAttachments = framebufferAttachments.data();
    framebufferCreateInfo.width = m_Info.Width;
    framebufferCreateInfo.height = m_Info.Height;
    framebufferCreateInfo.layers = 1;

    /* Create the framebuffer object. */
    SE_VULKAN_CHECK(vkCreateFramebuffer(g_VulkanDriver->GetDevice(), &framebufferCreateInfo, nullptr, &m_Handle));
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    /* Destroy the framebuffer object. */
    vkDestroyFramebuffer(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;
    m_Info = {};
}

}
