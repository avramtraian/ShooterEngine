// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

#include <vector>

namespace SE
{

struct VulkanRenderPassAttachment
{
    VkFormat Format;
    VkAttachmentLoadOp LoadOp;
    VkAttachmentStoreOp StoreOp;
    VkImageLayout InitialLayout;
    VkImageLayout FinalLayout;
};

struct VulkanRenderPassLayout
{
    std::vector<VulkanRenderPassAttachment> ColorAttachments;
    bool HasDepthStencilAttachment;
    VulkanRenderPassAttachment DepthStencilAttachment;
};

class VulkanRenderPass
{
    SE_MAKE_NONCOPYABLE(VulkanRenderPass);
    SE_MAKE_NONMOVABLE(VulkanRenderPass);

public:
    /* Utility functions that converts a generic render pass info into a vulkan render pass layout. */
    NODISCARD static VulkanRenderPassLayout GetLayoutFromInfo(const RenderPassInfo& info);

    /* Utility function that determines if two render pass layouts are compatible. */
    NODISCARD static bool CheckIfLayoutsAreCompatible(const VulkanRenderPassLayout& layoutA, const VulkanRenderPassLayout& layoutB);

public:
    VulkanRenderPass(const VulkanRenderPassLayout& layout);
    ~VulkanRenderPass();

    NODISCARD FORCEINLINE VkRenderPass GetHandle() const { return m_Handle; }
    NODISCARD FORCEINLINE const VulkanRenderPassLayout& GetLayout() const { return m_Layout; }

private:
    VkRenderPass m_Handle;
    VulkanRenderPassLayout m_Layout;
};

}
;