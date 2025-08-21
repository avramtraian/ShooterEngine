// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RenderPass.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanFramebuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanPipeline.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

class VulkanRenderPass : public RenderPass
{
public:
    VulkanRenderPass(const RenderPassInfo& info);
    ~VulkanRenderPass();

public:
    NODISCARD FORCEINLINE VkRenderPass GetHandle() const { return m_Handle; }
    NODISCARD FORCEINLINE bool HasDepthStencilAttachment() const { return m_HasDepthStencilAttachment; }
    NODISCARD FORCEINLINE const std::vector<RenderPassAttachment>& GetAttachments() const { return m_Attachments; }
    NODISCARD FORCEINLINE const RenderPassAttachment& GetAttachment(uint32 attachmentIndex) const
    {
        SE_ENSURE(attachmentIndex < m_Attachments.size());
        return m_Attachments[attachmentIndex];
    }

    NODISCARD FORCEINLINE uint32 GetAttachmentCount() const { return (uint32)m_Attachments.size(); }
    NODISCARD FORCEINLINE uint32 GetColorAttachmentCount() const
    {
        SE_ENSURE(!m_Attachments.empty());
        uint32 colorAttachmentCount = (uint32)m_Attachments.size();
        if (m_HasDepthStencilAttachment)
            colorAttachmentCount--;
        return colorAttachmentCount;
    }

public:
    NODISCARD VulkanFramebuffer* AcquireCompatibleFramebuffer(const RenderPassBeginInfo& beginInfo);
    NODISCARD RefPtr<VulkanPipeline> AcquireCompatiblePipeline(const GraphicsState& graphicsState);

private:
    VkRenderPass m_Handle;
    std::vector<RenderPassAttachment> m_Attachments;
    bool m_HasDepthStencilAttachment;

    struct CachedFramebuffer
    {
        std::unique_ptr<VulkanFramebuffer> Framebuffer;
        uint32 NumberOfFramesSinceLastUse { 0 };
    };

    struct CachedPipeline
    {
        RefPtr<VulkanPipeline> Pipeline;
        uint32 NumberOfFramesSinceLastUse { 0 };
    };

    std::vector<CachedFramebuffer> m_CachedFramebuffers;
    std::vector<CachedPipeline> m_CachedPipelines;
};

}
