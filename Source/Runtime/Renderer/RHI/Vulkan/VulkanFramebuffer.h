// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/LockPtr.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

class VulkanFramebuffer final : public LockCounted
{
public:
    VulkanFramebuffer(const WeakRefPtr<VulkanRenderPass>& parentRenderPass);
    ~VulkanFramebuffer();

    void Invalidate(const Vector<RefPtr<Texture2D>>& textures, VkRenderPass renderPassHandle);
    void Destroy();
    NODISCARD ALWAYS_INLINE bool IsValid() const { return (m_Handle != VK_NULL_HANDLE); }

    NODISCARD bool IsCompatibleWithRenderPassBeginInfo(const RenderPassBeginInfo& beginInfo) const;

public:
    NODISCARD ALWAYS_INLINE VkFramebuffer GetHandle() const { return m_Handle; }

    NODISCARD ALWAYS_INLINE uint32 GetSizeX() const
    {
        SE_ENSURE(m_Textures.HasElements());
        return m_Textures.First()->GetSizeX();
    }

    NODISCARD ALWAYS_INLINE uint32 GetSizeY() const
    {
        SE_ENSURE(m_Textures.HasElements());
        return m_Textures.First()->GetSizeY();
    }

protected:
    virtual void OnLock() override;
    virtual void OnUnlock() override;

private:
    VkFramebuffer m_Handle;

    WeakRefPtr<VulkanRenderPass> m_ParentRenderPass;
    StrongRefPtr<VulkanRenderPass> m_LockedParentRenderPass;

    Vector<WeakRefPtr<VulkanTexture2D>> m_Textures;
    Vector<RHIObjectCallback> m_TexturePreDestroyCallbacks;
    Vector<StrongRefPtr<VulkanTexture2D>> m_LockedTextures;
};

}
