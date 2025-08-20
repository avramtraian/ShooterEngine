// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Core/Containers/RefPtr.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

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
    VulkanSwapchain(VkSurfaceKHR surface, uint32 sizeX, uint32 sizeY, uint32 imageCount, const VulkanSwapchain* oldSwapchain);
    virtual ~VulkanSwapchain() override;

    NODISCARD FORCEINLINE VkSwapchainKHR GetHandle() const { return m_Handle; }
    NODISCARD FORCEINLINE uint32 GetSizeX() const { return m_SizeX; }
    NODISCARD FORCEINLINE uint32 GetSizeY() const { return m_SizeY; }
    NODISCARD FORCEINLINE const ImmutableProperties& GetImmutableProperties() const { return m_ImmutableProperties; }
    
    NODISCARD FORCEINLINE uint32 GetImageCount() const { return (uint32)m_Images.size(); }
    NODISCARD FORCEINLINE const std::vector<VkImage>& GetImages() const { return m_Images; }
    NODISCARD FORCEINLINE const std::vector<VkImageView>& GetImageViews() const { return m_ImageViews; }

private:
    VkPresentModeKHR FindBestPresentMode(VkSurfaceKHR surface) const;

private:
    VkSwapchainKHR m_Handle;
    ImmutableProperties m_ImmutableProperties;
    uint32 m_SizeX;
    uint32 m_SizeY;
    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;
};

}
