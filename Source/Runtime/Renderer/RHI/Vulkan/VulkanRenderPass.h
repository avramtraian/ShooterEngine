// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Renderer/RHI/RenderPass.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanFramebuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanPipeline.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

class VulkanRenderPass final : public RenderPass
{
public:
    VulkanRenderPass(const RenderPassInfo& info);
    ~VulkanRenderPass();

public:
    NODISCARD ALWAYS_INLINE VkRenderPass GetHandle() const { return m_Handle; }
    NODISCARD ALWAYS_INLINE bool HasDepthStencilAttachment() const { return m_HasDepthStencilAttachment; }
    NODISCARD ALWAYS_INLINE const Vector<RenderPassAttachment>& GetAttachments() const { return m_Attachments; }
    NODISCARD ALWAYS_INLINE const RenderPassAttachment& GetAttachment(uint32 attachmentIndex) const
    {
        SE_ENSURE(attachmentIndex < m_Attachments.Count());
        return m_Attachments[attachmentIndex];
    }

    NODISCARD ALWAYS_INLINE uint32 GetAttachmentCount() const { return (uint32)m_Attachments.Count(); }
    NODISCARD ALWAYS_INLINE uint32 GetColorAttachmentCount() const
    {
        SE_ENSURE(!m_Attachments.IsEmpty());
        uint32 colorAttachmentCount = (uint32)m_Attachments.Count();
        if (m_HasDepthStencilAttachment)
            colorAttachmentCount--;
        return colorAttachmentCount;
    }

public:
    NODISCARD VulkanFramebuffer* AcquireCompatibleFramebuffer(const RenderPassBeginInfo& beginInfo);
    NODISCARD VulkanPipeline* AcquireCompatiblePipeline(const GraphicsState& graphicsState, const RefPtr<Shader>& shader);

private:
    VkRenderPass m_Handle;
    Vector<RenderPassAttachment> m_Attachments;
    bool m_HasDepthStencilAttachment;

    struct CachedFramebuffer
    {
        OwnPtr<VulkanFramebuffer> Framebuffer;
        uint32 NumberOfFramesSinceLastUse { 0 };
    };

    struct CachedPipeline
    {
        OwnPtr<VulkanPipeline> Pipeline;
        uint32 NumberOfFramesSinceLastUse { 0 };
    };

    Vector<CachedFramebuffer> m_CachedFramebuffers;
    Vector<CachedPipeline> m_CachedPipelines;
};

}
