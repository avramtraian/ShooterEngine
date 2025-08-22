// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/LockPtr.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

#include <memory>
#include <vector>

namespace SE
{

class VulkanFramebuffer : public LockCounted
{
public:
    VulkanFramebuffer(const WeakRefPtr<VulkanRenderPass>& parentRenderPass);
    ~VulkanFramebuffer();

    void Invalidate(const std::vector<RefPtr<Texture2D>>& textures, VkRenderPass renderPassHandle);
    void Destroy();
    NODISCARD FORCEINLINE bool IsValid() const { return (m_Handle != VK_NULL_HANDLE); }

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

protected:
    virtual void OnLock() override;
    virtual void OnUnlock() override;

private:
    VkFramebuffer m_Handle;

    WeakRefPtr<VulkanRenderPass> m_ParentRenderPass;
    StrongRefPtr<VulkanRenderPass> m_LockedParentRenderPass;

    std::vector<WeakRefPtr<VulkanTexture2D>> m_Textures;
    std::vector<RHIObjectCallback> m_TexturePreDestroyCallbacks;
    std::vector<StrongRefPtr<VulkanTexture2D>> m_LockedTextures;
};

}
