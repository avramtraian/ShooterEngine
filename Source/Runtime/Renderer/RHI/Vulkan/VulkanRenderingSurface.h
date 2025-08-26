// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanSwapchain.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// VULKAN SURFACE DECLARATION. //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VulkanSurface : public RefCounted
{
public:
    VulkanSurface(const RefPtr<Window>& targetWindow);
    virtual ~VulkanSurface() override;

    NODISCARD FORCEINLINE VkSurfaceKHR GetHandle() const { return m_Handle; }
    NODISCARD FORCEINLINE uint32 GetSizeX() const { return m_TargetWindow->GetSizeX(); }
    NODISCARD FORCEINLINE uint32 GetSizeY() const { return m_TargetWindow->GetSizeY(); }

private:
    VkSurfaceKHR m_Handle;
    RefPtr<Window> m_TargetWindow;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// VULKAN RENDERING SURFACE DECLARATION. /////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

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
    virtual void EndFrame(bool waitForRenderFinishedSemaphore) override;

    virtual SemaphoreHandle GetImageAvailableSemaphore() override;
    virtual SemaphoreHandle GetRenderFinishedSemaphore() override;
    virtual FenceHandle GetRenderFinishedFence() override;

private:
    RefPtr<VulkanSurface> m_Surface;
    uint32 m_SwapchainMinImageCount;
    RefPtr<VulkanSwapchain> m_Swapchain;
    std::vector<RefPtr<VulkanSwapchainTexture2D>> m_SwapchainTextures;

    uint32 m_MaxFramesInFlight;
    uint32 m_CurrentFrameIndex;
    uint32 m_CurrentSwapchainImageIndex;

    std::vector<FenceHandle> m_RenderFinishedFences;
};

}
