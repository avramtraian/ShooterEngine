// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanFramebuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

VulkanFramebuffer::VulkanFramebuffer(const std::vector<RefPtr<Texture2D>>& textures, VkRenderPass renderPassHandle)
    : m_Handle(VK_NULL_HANDLE)
{
    if (textures.empty())
    {
        SE_LOG_ERROR("Trying to create a [Vulkan] framebuffer with no attachments!");
        return;
    }

    std::vector<VkImageView> framebufferAttachments;
    framebufferAttachments.reserve(textures.size());
    m_Textures.reserve(textures.size());

    const uint32 framebufferWidth = textures.front()->GetSizeX();
    const uint32 framebufferHeight = textures.front()->GetSizeY();

    for (const RefPtr<Texture2D>& texture : textures)
    {
        /* NOTE(Traian): Vulkan framebuffers are created by the render pass on demand. The most likely cause of these errors is an invalid
         * render pass begin info structure passed to the 'CommandList::BeginRenderPass' function. However, these errors should have been
         * caught at the previously mentioned API level, and not in the framebuffer creation code. */
        SE_ENSURE(texture.IsValid());
        SE_ENSURE(texture->GetSizeX() == framebufferWidth && texture->GetSizeY() == framebufferHeight);

        auto vulkanTexture = texture.As<VulkanTexture2D>();
        m_Textures.push_back(vulkanTexture);
        framebufferAttachments.push_back(vulkanTexture->GetHandle().View);

    }

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPassHandle;
    framebufferCreateInfo.attachmentCount = (uint32)framebufferAttachments.size();
    framebufferCreateInfo.pAttachments = framebufferAttachments.data();
    framebufferCreateInfo.width = framebufferWidth;
    framebufferCreateInfo.height = framebufferHeight;
    framebufferCreateInfo.layers = 1;

    /* Create the framebuffer object. */
    SE_VULKAN_CHECK(vkCreateFramebuffer(g_VulkanDriver->GetDevice(), &framebufferCreateInfo, nullptr, &m_Handle));
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    /* Destroy the framebuffer object. */
    vkDestroyFramebuffer(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;

    /* Release all held references for the framebuffer textures. */
    m_Textures.clear();
}

bool VulkanFramebuffer::IsCompatibleWithRenderPassBeginInfo(const RenderPassBeginInfo& beginInfo) const
{
    const bool hasDepthStencilAttachment = beginInfo.DepthStencilAttachmentTexture.Texture.IsValid();
    uint32 attachmentCount = (uint32)beginInfo.ColorAttachmentTextures.size();
    if (hasDepthStencilAttachment)
        attachmentCount++;

    if (attachmentCount != m_Textures.size())
        return false;

    for (const auto& attachmentTextureIt : beginInfo.ColorAttachmentTextures)
    {
        const uint32 attachmentIndex = attachmentTextureIt.first;
        const RenderPassAttachmentTexture& attachmentTexture = attachmentTextureIt.second;

        if (attachmentIndex >= m_Textures.size())
            return false;
        if (m_Textures[attachmentIndex].Get() != attachmentTexture.Texture.Get())
            return false;
    }

    if (hasDepthStencilAttachment && (m_Textures.back().Get() != beginInfo.DepthStencilAttachmentTexture.Texture.Get()))
        return false;

    return true;
}

}
