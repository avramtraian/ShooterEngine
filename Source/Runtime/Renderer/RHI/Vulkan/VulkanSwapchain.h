// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Core/Containers/RefPtr.h>
#include <Runtime/Renderer/RHI/Synchronization.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

// Forward declarations.
class VulkanSwapchain;
class VulkanSurface;

struct VulkanSwapchainInfo
{
public:
    uint32                  MinImageCount     { 0 };
    uint32                  MaxFramesInFlight { 0 };
    RefPtr<VulkanSurface>   Surface;
    RefPtr<VulkanSwapchain> OldSwapchain;

public:
    inline VulkanSwapchainInfo& SetMinImageCount     (uint32 minImageCount)                 { MinImageCount = minImageCount;         return *this; }
    inline VulkanSwapchainInfo& SetMaxFramesInFlight (uint32 maxFramesInFlight)             { MaxFramesInFlight = maxFramesInFlight; return *this; }
    inline VulkanSwapchainInfo& SetSurface           (RefPtr<VulkanSurface> surface)        { Surface = Move(surface);               return *this; }
    inline VulkanSwapchainInfo& SetOldSwapchain      (RefPtr<VulkanSwapchain> oldSwapchain) { OldSwapchain = Move(oldSwapchain);     return *this; }
};

class VulkanSwapchain : public RefCounted
{
public:
    struct ImmutableProperties
    {
        VkFormat         Format      { VK_FORMAT_UNDEFINED };
        VkColorSpaceKHR  ColorSpace  { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        VkPresentModeKHR PresentMode { VK_PRESENT_MODE_FIFO_KHR };
    };

public:
    VulkanSwapchain(const VulkanSwapchainInfo& info);
    virtual ~VulkanSwapchain() override;

    NODISCARD FORCEINLINE VkSwapchainKHR GetHandle() const { return m_Handle; }
    NODISCARD FORCEINLINE uint32 GetSizeX() const { return m_SizeX; }
    NODISCARD FORCEINLINE uint32 GetSizeY() const { return m_SizeY; }
    NODISCARD FORCEINLINE const ImmutableProperties& GetImmutableProperties() const { return m_ImmutableProperties; }
    
    NODISCARD FORCEINLINE uint32 GetImageCount() const { return (uint32)m_Images.Count(); }
    NODISCARD FORCEINLINE const Vector<VkImage>& GetImages() const { return m_Images; }
    NODISCARD FORCEINLINE const Vector<VkImageView>& GetImageViews() const { return m_ImageViews; }

    NODISCARD FORCEINLINE const Vector<SemaphoreHandle>& GetImageAvailableSemaphores() const { return m_ImageAvailableSemaphores; }
    NODISCARD FORCEINLINE SemaphoreHandle GetImageAvailableSemaphore(uint32 frameIndex) const
    {
        SE_ASSERT(frameIndex < m_ImageAvailableSemaphores.Count());
        return m_ImageAvailableSemaphores.At(frameIndex);
    }

    NODISCARD FORCEINLINE const Vector<SemaphoreHandle>& GetRenderFinishedSemaphores() const { return m_RenderFinishedSemaphores; }
    NODISCARD FORCEINLINE SemaphoreHandle GetRenderFinishedSemaphore(uint32 frameIndex) const
    {
        SE_ASSERT(frameIndex < m_RenderFinishedSemaphores.Count());
        return m_RenderFinishedSemaphores.At(frameIndex);
    }

private:
    VkPresentModeKHR FindBestPresentMode(VkSurfaceKHR surface) const;

private:
    VkSwapchainKHR m_Handle;
    RefPtr<VulkanSurface> m_Surface;
    
    ImmutableProperties m_ImmutableProperties;
    uint32 m_SizeX;
    uint32 m_SizeY;

    Vector<VkImage> m_Images;
    Vector<VkImageView> m_ImageViews;

    Vector<SemaphoreHandle> m_ImageAvailableSemaphores;
    Vector<SemaphoreHandle> m_RenderFinishedSemaphores;
};

}
