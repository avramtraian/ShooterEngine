// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/LockPtr.h>
#include <Runtime/Renderer/RHI/GraphicsState.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanShader.h>

namespace SE
{

class VulkanPipeline final : public LockCounted
{
public:
    VulkanPipeline(const WeakRefPtr<VulkanRenderPass>& parentRenderPass);
    ~VulkanPipeline();

public:
    NODISCARD VkPipeline GetHandle() const;
    NODISCARD const GraphicsState& GetGraphicsState() const;
    NODISCARD RefPtr<VulkanShader> GetShader() const;

    void Invalidate(const GraphicsState& graphicsState, const RefPtr<Shader>& shader, VkRenderPass renderPassHandle, uint32 colorAttachmentCount);
    void Destroy();
    NODISCARD FORCEINLINE bool IsValid() const { return (m_Handle != VK_NULL_HANDLE); }

    NODISCARD bool IsCompatibleWithGraphicsStateAndShader(const GraphicsState& graphicsState, const RefPtr<Shader>& shader) const;

protected:
    virtual void OnLock() override;
    virtual void OnUnlock() override;

private:
    VkPipeline m_Handle;
    GraphicsState m_GraphicsState;

    WeakRefPtr<VulkanRenderPass> m_ParentRenderPass;
    StrongRefPtr<VulkanRenderPass> m_LockedParentRenderPass;

    WeakRefPtr<VulkanShader> m_Shader;
    StrongRefPtr<VulkanShader> m_LockedShader;
    RHIObjectCallback m_ShaderPreDestroyCallback;
};

}
