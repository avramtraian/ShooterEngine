// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

#include <vector>

namespace SE
{

struct VulkanFramebufferInfo
{
    uint32 Width            { 0 };
    uint32 Height           { 0 };
    VkRenderPass RenderPass { VK_NULL_HANDLE };
    std::vector<std::shared_ptr<VulkanTexture2D>> Attachments;
};

class VulkanFramebuffer
{
    SE_MAKE_NONCOPYABLE(VulkanFramebuffer);
    SE_MAKE_NONMOVABLE(VulkanFramebuffer);

public:
    /* Utility function that creates a vulkan framebuffer info structure from a render pass description. */
    NODISCARD static VulkanFramebufferInfo GetInfoFromRenderPass(VkRenderPass renderPassHandle, const RenderPassInfo& renderPassInfo);

    /* Utility functions that checks if two framebuffers structures are compatible. */
    NODISCARD static bool CheckIfFramebuffersAreCompatible(const VulkanFramebufferInfo& infoA, const VulkanFramebufferInfo& infoB);

public:
    VulkanFramebuffer(const VulkanFramebufferInfo& info);
    ~VulkanFramebuffer();

    NODISCARD FORCEINLINE VkFramebuffer GetHandle() const { return m_Handle; }
    NODISCARD FORCEINLINE VkRenderPass GetRenderPass() const { return m_Info.RenderPass; }
    NODISCARD FORCEINLINE const VulkanFramebufferInfo& GetInfo() const { return m_Info; }

private:
    VkFramebuffer m_Handle;
    VulkanFramebufferInfo m_Info;
};

}
