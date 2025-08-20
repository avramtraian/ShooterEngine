// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Core/Containers/RefPtr.h>
#include <Runtime/Renderer/RHI/Synchronization.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

// Forward declaration.
class VulkanSwapchain;

struct VulkanSwapchainInfo
{
public:
    VkSurfaceKHR            Surface           { VK_NULL_HANDLE };
    uint32                  SizeX             { 0 };
    uint32                  SizeY             { 0 };
    uint32                  MinImageCount     { 0 };
    uint32                  MaxFramesInFlight { 0 };
    RefPtr<VulkanSwapchain> OldSwapchain      { nullptr };

public:
    inline VulkanSwapchainInfo& SetSurface           (VkSurfaceKHR surface)                 { Surface = surface;                      return *this; }
    inline VulkanSwapchainInfo& SetSizeX             (uint32 sizeX)                         { SizeX = sizeX;                          return *this; }
    inline VulkanSwapchainInfo& SetSizeY             (uint32 sizeY)                         { SizeY = sizeY;                          return *this; }
    inline VulkanSwapchainInfo& SetSize              (uint32 sizeX, uint32 sizeY)           { SizeX = sizeX; SizeY = sizeY;           return *this; }
    inline VulkanSwapchainInfo& SetMinImageCount     (uint32 minImageCount)                 { MinImageCount = minImageCount;          return *this; }
    inline VulkanSwapchainInfo& SetMaxFramesInFlight (uint32 maxFramesInFlight)             { MaxFramesInFlight = maxFramesInFlight;  return *this; }
    inline VulkanSwapchainInfo& SetOldSwapchain      (RefPtr<VulkanSwapchain> oldSwapchain) { OldSwapchain = std::move(oldSwapchain); return *this; }
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
    
    NODISCARD FORCEINLINE uint32 GetImageCount() const { return (uint32)m_Images.size(); }
    NODISCARD FORCEINLINE const std::vector<VkImage>& GetImages() const { return m_Images; }
    NODISCARD FORCEINLINE const std::vector<VkImageView>& GetImageViews() const { return m_ImageViews; }

    NODISCARD FORCEINLINE const std::vector<SemaphoreHandle>& GetImageAvailableSemaphores() const { return m_ImageAvailableSemaphores; }
    NODISCARD FORCEINLINE SemaphoreHandle GetImageAvailableSemaphore(uint32 frameIndex) const
    {
        SE_ASSERT(frameIndex < m_ImageAvailableSemaphores.size());
        return m_ImageAvailableSemaphores.at(frameIndex);
    }

    NODISCARD FORCEINLINE const std::vector<SemaphoreHandle>& GetRenderFinishedSemaphores() const { return m_RenderFinishedSemaphores; }
    NODISCARD FORCEINLINE SemaphoreHandle GetRenderFinishedSemaphore(uint32 frameIndex) const
    {
        SE_ASSERT(frameIndex < m_RenderFinishedSemaphores.size());
        return m_RenderFinishedSemaphores.at(frameIndex);
    }

private:
    VkPresentModeKHR FindBestPresentMode(VkSurfaceKHR surface) const;

private:
    VkSwapchainKHR m_Handle;
    
    ImmutableProperties m_ImmutableProperties;
    uint32 m_SizeX;
    uint32 m_SizeY;

    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;

    std::vector<SemaphoreHandle> m_ImageAvailableSemaphores;
    std::vector<SemaphoreHandle> m_RenderFinishedSemaphores;
};

}
