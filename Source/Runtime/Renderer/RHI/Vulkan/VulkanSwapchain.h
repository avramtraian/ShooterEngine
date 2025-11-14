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

    NODISCARD ALWAYS_INLINE VkSwapchainKHR GetHandle() const { return m_Handle; }
    NODISCARD ALWAYS_INLINE uint32 GetSizeX() const { return m_SizeX; }
    NODISCARD ALWAYS_INLINE uint32 GetSizeY() const { return m_SizeY; }
    NODISCARD ALWAYS_INLINE const ImmutableProperties& GetImmutableProperties() const { return m_ImmutableProperties; }
    
    NODISCARD ALWAYS_INLINE uint32 GetImageCount() const { return static_cast<uint32>(m_Images.Count()); }
    NODISCARD ALWAYS_INLINE const Vector<VkImage>& GetImages() const { return m_Images; }
    NODISCARD ALWAYS_INLINE const Vector<VkImageView>& GetImageViews() const { return m_ImageViews; }

    NODISCARD ALWAYS_INLINE const Vector<SemaphoreHandle>& GetImageAvailableSemaphores() const { return m_ImageAvailableSemaphores; }
    NODISCARD ALWAYS_INLINE SemaphoreHandle GetImageAvailableSemaphore(uint32 frameIndex) const
    {
        SE_ASSERT(frameIndex < m_ImageAvailableSemaphores.Count());
        return m_ImageAvailableSemaphores.At(frameIndex);
    }

    NODISCARD ALWAYS_INLINE const Vector<SemaphoreHandle>& GetRenderFinishedSemaphores() const { return m_RenderFinishedSemaphores; }
    NODISCARD ALWAYS_INLINE SemaphoreHandle GetRenderFinishedSemaphore(uint32 imageIndex) const
    {
        SE_ASSERT(imageIndex < m_RenderFinishedSemaphores.Count());
        return m_RenderFinishedSemaphores.At(imageIndex);
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
