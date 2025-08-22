// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/GraphicsState.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanShader.h>

#include <memory>

namespace SE
{

class VulkanPipeline
{
public:
    VulkanPipeline(const WeakRefPtr<VulkanRenderPass>& parentRenderPass);
    ~VulkanPipeline();

public:
    NODISCARD VkPipeline GetHandle() const;
    NODISCARD const GraphicsState& GetGraphicsState() const;
    NODISCARD RefPtr<VulkanShader> GetShader() const;

    NODISCARD FORCEINLINE uint32 GetLockCount() const { return m_LockCount; }
    NODISCARD FORCEINLINE bool IsLocked() const { return (m_LockCount > 0); }
    NODISCARD FORCEINLINE bool IsUnlocked() const { return (m_LockCount == 0); }

    void Invalidate(const GraphicsState& graphicsState, const RefPtr<Shader>& shader, VkRenderPass renderPassHandle, uint32 colorAttachmentCount);
    void Destroy();
    NODISCARD FORCEINLINE bool IsValid() const { return (m_Handle != VK_NULL_HANDLE); }

    void IncrementLockCount();
    void DecrementLockCount();

    NODISCARD bool IsCompatibleWithGraphicsStateAndShader(const GraphicsState& graphicsState, const RefPtr<Shader>& shader) const;

private:
    VkPipeline m_Handle;
    uint32 m_LockCount;
    GraphicsState m_GraphicsState;

    WeakRefPtr<VulkanRenderPass> m_ParentRenderPass;
    StrongRefPtr<VulkanRenderPass> m_LockedParentRenderPass;

    WeakRefPtr<VulkanShader> m_Shader;
    StrongRefPtr<VulkanShader> m_LockedShader;
    RHIObjectCallback m_ShaderPreDestroyCallback;
};

}
