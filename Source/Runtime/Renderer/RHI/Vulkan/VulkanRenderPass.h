// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RenderPass.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
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

    NODISCARD FORCEINLINE VkFramebuffer GetFramebuffer() const { return m_Framebuffer; }
    NODISCARD FORCEINLINE uint32 GetFramebufferSizeX() const { return m_Attachments.front().Texture->GetSizeX(); }
    NODISCARD FORCEINLINE uint32 GetFramebufferSizeY() const { return m_Attachments.front().Texture->GetSizeY(); }

public:
    NODISCARD VulkanPipeline* AcquireCompatiblePipeline(const GraphicsState& graphicsState);

private:
    VkRenderPass m_Handle;
    VkFramebuffer m_Framebuffer;
    std::vector<RenderPassAttachment> m_Attachments;
    bool m_HasDepthStencilAttachment;

    std::vector<std::unique_ptr<VulkanPipeline>> m_Pipelines;
};

}
