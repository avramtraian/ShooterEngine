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
public:
    VulkanFramebuffer();
    ~VulkanFramebuffer();

    void Invalidate(const std::vector<RefPtr<Texture2D>>& textures, VkRenderPass renderPassHandle);
    void Destroy();
    NODISCARD FORCEINLINE bool IsValid() const { return (m_Handle != VK_NULL_HANDLE); }

    NODISCARD bool IsCompatibleWithRenderPassBeginInfo(const RenderPassBeginInfo& beginInfo) const;

    void IncrementLockCount();
    void DecrementLockCount();

public:
    NODISCARD FORCEINLINE VkFramebuffer GetHandle() const { return m_Handle; }

    NODISCARD FORCEINLINE uint32 GetLockCount() const { return m_LockCount; }
    NODISCARD FORCEINLINE bool IsLocked() const { return (m_LockCount > 0); }
    NODISCARD FORCEINLINE bool IsUnlocked() const { return (m_LockCount == 0); }

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
    uint32 m_LockCount;

    std::vector<WeakRefPtr<VulkanTexture2D>> m_Textures;
    std::vector<ShaderResourcePreDestroyCallback> m_TexturePreDestroyCallbacks;
    std::vector<StrongRefPtr<VulkanTexture2D>> m_LockedTextures;
};

}
