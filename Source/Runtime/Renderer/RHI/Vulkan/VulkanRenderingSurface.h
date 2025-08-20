// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanSwapchain.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

class VulkanRenderingSurface : public RenderingSurface
{
public:
    VulkanRenderingSurface(const RenderingSurfaceInfo& info);
    virtual ~VulkanRenderingSurface() override;

public:
    NODISCARD FORCEINLINE virtual uint32 GetMaxFramesInFlight() const override { return m_MaxFramesInFlight; }
    NODISCARD FORCEINLINE virtual uint32 GetCurrentFrameIndex() const override { return m_CurrentFrameIndex; }
    NODISCARD FORCEINLINE virtual uint32 GetCurrentSwapchainImageIndex() const override { return m_CurrentSwapchainImageIndex; }

    NODISCARD FORCEINLINE virtual TextureFormat GetSurfaceFormat() const override { return TextureFormatFromVulkan(m_Swapchain->GetImmutableProperties().Format); }
    NODISCARD FORCEINLINE virtual uint32 GetSurfaceSizeX() const override { return m_Swapchain->GetSizeX(); }
    NODISCARD FORCEINLINE virtual uint32 GetSurfaceSizeY() const override { return m_Swapchain->GetSizeY(); }

    virtual bool Invalidate() override;

    virtual RefPtr<Texture2D> GetSurfaceTexture2D(uint32 imageIndex) override;
    virtual RefPtr<Texture2D> GetCurrentSurfaceTexture2D() override { return GetSurfaceTexture2D(m_CurrentSwapchainImageIndex); }

    virtual void BeginFrame() override;
    virtual void EndFrame() override;

    FORCEINLINE virtual SemaphoreHandle GetImageAvailableSemaphore() override { return m_Swapchain->GetImageAvailableSemaphore(m_CurrentFrameIndex); }
    FORCEINLINE virtual SemaphoreHandle GetRenderFinishedSemaphore() override { return m_Swapchain->GetRenderFinishedSemaphore(m_CurrentFrameIndex); }
    FORCEINLINE virtual FenceHandle GetRenderFinishedFence() override { return m_RenderFinishedFences[m_CurrentFrameIndex]; }

private:
    Window* m_OwningWindow;
    VkSurfaceKHR m_Surface;
    uint32 m_SwapchainMinImageCount;
    RefPtr<VulkanSwapchain> m_Swapchain;
    std::vector<RefPtr<VulkanSwapchainTexture2D>> m_SwapchainTextures;

    uint32 m_MaxFramesInFlight;
    uint32 m_CurrentFrameIndex;
    uint32 m_CurrentSwapchainImageIndex;

    std::vector<FenceHandle> m_RenderFinishedFences;
};

}
