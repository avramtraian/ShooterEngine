// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

#include <memory>
#include <vector>

namespace SE
{

class VulkanFramebuffer
{
    SE_MAKE_NONCOPYABLE(VulkanFramebuffer);
    SE_MAKE_NONMOVABLE(VulkanFramebuffer);

public:
    VulkanFramebuffer(const std::vector<RefPtr<Texture2D>>& textures, VkRenderPass renderPassHandle);
    ~VulkanFramebuffer();

    NODISCARD bool IsCompatibleWithRenderPassBeginInfo(const RenderPassBeginInfo& beginInfo) const;

public:
    NODISCARD FORCEINLINE VkFramebuffer GetHandle() const { return m_Handle; }

    NODISCARD FORCEINLINE uint32 GetSizeX() const
    {
        SE_ENSURE(!m_Textures.empty());
        return m_Textures.front()->GetSizeX();
    }

    NODISCARD FORCEINLINE uint32 GetSizeY() const
    {
        SE_ENSURE(!m_Textures.empty());
        return m_Textures.front()->GetSizeY();
    }

private:
    VkFramebuffer m_Handle;
    std::vector<RefPtr<VulkanTexture2D>> m_Textures;
};

}
