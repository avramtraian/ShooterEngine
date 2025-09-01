// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanFramebuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

VulkanFramebuffer::VulkanFramebuffer(const WeakRefPtr<VulkanRenderPass>& parentRenderPass)
    : m_Handle(VK_NULL_HANDLE)
    , m_ParentRenderPass(parentRenderPass)
{}

VulkanFramebuffer::~VulkanFramebuffer()
{
    Destroy();
}

void VulkanFramebuffer::Invalidate(const Vector<RefPtr<Texture2D>>& textures, VkRenderPass renderPassHandle)
{
    if (IsLocked())
    {
        SE_LOG_ERROR("Trying to invalidate a Vulkan framebuffer that is locked!");
        SE_ASSERT_NOT_REACHED;
        return;
    }

    // Destroy the previous instance of the framebuffer.
    Destroy();

    if (textures.IsEmpty())
    {
        SE_LOG_ERROR("Trying to create a [Vulkan] framebuffer with no attachments!");
        return;
    }

    Vector<VkImageView> framebufferAttachments;
    framebufferAttachments.EnsureCapacity(textures.Count());
    m_Textures.EnsureCapacity(textures.Count());

    const uint32 framebufferWidth = textures.First()->GetSizeX();
    const uint32 framebufferHeight = textures.First()->GetSizeY();

    for (const RefPtr<Texture2D>& texture : textures)
    {
        // NOTE(Traian): Vulkan framebuffers are created by the render pass on demand. The most likely cause of these errors is an invalid
        // render pass begin info structure passed to the 'CommandList::BeginRenderPass' function. However, these errors should have been
        // caught At the previously mentioned API level, and not in the framebuffer creation code.
        SE_ENSURE(texture.IsValid());
        SE_ENSURE(texture->GetSizeX() == framebufferWidth && texture->GetSizeY() == framebufferHeight);

        auto vulkanTexture = texture.As<VulkanTexture2D>();
        m_Textures.Add(vulkanTexture);
        framebufferAttachments.Add(vulkanTexture->GetHandle().View);

        // Set the pre-destroy callbacks for textures.
        m_TexturePreDestroyCallbacks.Add(vulkanTexture->AddCallback(RHIObjectCallbackType::PreDestroy,
            [this](RHIObject& resource)
            {
                // NOTE(Traian): This should never happen because as long as the framebuffer is locked it holds strong references to the
                // used textures, and thus the pre-destroy callback should never be dispatched. If this assert is triggered, it means that
                // 'm_Textures' and 'm_LockedTextures' don't match anymore (which would be an internal error).
                SE_ASSERT(IsUnlocked());
                Destroy();
            }
        ));
    }

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPassHandle;
    framebufferCreateInfo.attachmentCount = (uint32)framebufferAttachments.Count();
    framebufferCreateInfo.pAttachments = framebufferAttachments.Elements();
    framebufferCreateInfo.width = framebufferWidth;
    framebufferCreateInfo.height = framebufferHeight;
    framebufferCreateInfo.layers = 1;

    // Create the framebuffer object.
    SE_VULKAN_CHECK(vkCreateFramebuffer(g_VulkanDriver->GetDevice(), &framebufferCreateInfo, nullptr, &m_Handle));
}

void VulkanFramebuffer::Destroy()
{
    if (IsLocked())
    {
        SE_LOG_ERROR("Trying to destroy a Vulkan framebuffer that is locked!");
        SE_ASSERT_NOT_REACHED;
        return;
    }

    // Destroy the framebuffer object.
    vkDestroyFramebuffer(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;

    // Release all held references for the framebuffer textures.
    m_Textures.ClearAndShrink();
    m_TexturePreDestroyCallbacks.ClearAndShrink();
}

bool VulkanFramebuffer::IsCompatibleWithRenderPassBeginInfo(const RenderPassBeginInfo& beginInfo) const
{
    const bool hasDepthStencilAttachment = beginInfo.DepthStencilAttachmentTexture.Texture.IsValid();
    uint32 attachmentCount = (uint32)beginInfo.ColorAttachmentTextures.Count();
    if (hasDepthStencilAttachment)
        attachmentCount++;

    if (attachmentCount != m_Textures.Count())
        return false;

    for (const auto& [attachmentIndex, attachmentTexture] : beginInfo.ColorAttachmentTextures)
    {
        if (attachmentIndex >= m_Textures.Count())
            return false;
        if (m_Textures[attachmentIndex].Get() != attachmentTexture.Texture.Get())
            return false;
    }

    if (hasDepthStencilAttachment && (m_Textures.Last().Get() != beginInfo.DepthStencilAttachmentTexture.Texture.Get()))
        return false;

    return true;
}

void VulkanFramebuffer::OnLock()
{
    // Acquire strong reference for the parent render pass.
    SE_ASSERT(m_ParentRenderPass.IsValid());
    m_LockedParentRenderPass = m_ParentRenderPass;

    // Acquire strong references for the textures.
    SE_ASSERT(m_LockedTextures.IsEmpty());
    m_LockedTextures.EnsureCapacity(m_Textures.Count());

    for (const auto& texture : m_Textures)
    {
        // NOTE(Traian): Since the framebuffer can only be locked by a command list when the owning render pass
        // is bound the texture is _always_ strong referenced by the 'RenderPassBeginInfo' structure (otherwise
        // this framebuffer wouldn't be selected).
        SE_ASSERT(texture.IsValid());
        m_LockedTextures.Add(texture);
    }
}

void VulkanFramebuffer::OnUnlock()
{
    // Release the strong references for the textures.
    m_LockedTextures.Clear();

    // Release the strong reference for the parent render pass.
    m_LockedParentRenderPass.Release();
}

}
