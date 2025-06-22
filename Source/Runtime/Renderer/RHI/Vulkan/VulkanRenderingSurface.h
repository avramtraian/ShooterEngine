// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

class VulkanRenderingSurface : public RenderingSurface
{
public:
    struct Swapchain
    {
        VkSwapchainKHR       Handle      { VK_NULL_HANDLE };
        uint32               SizeX       { 0 };
        uint32               SizeY       { 0 };
        VkFormat             Format      { VK_FORMAT_UNDEFINED };
        VkColorSpaceKHR      ColorSpace  { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        VkPresentModeKHR     PresentMode { VK_PRESENT_MODE_FIFO_KHR };
        std::vector<VkImage> Images;
    };

public:
    VulkanRenderingSurface(const RenderingSurfaceInfo& info);
    virtual ~VulkanRenderingSurface() override;

public:
    NODISCARD FORCEINLINE virtual uint32 GetSurfaceSizeX() const override { return m_Swapchain.SizeX; }
    NODISCARD FORCEINLINE virtual uint32 GetSurfaceSizeY() const override { return m_Swapchain.SizeY; }

    virtual bool Invalidate() override;
    void Destroy();

public:
    NODISCARD FORCEINLINE VkSurfaceKHR GetSurface() const { return m_Surface; }
    NODISCARD FORCEINLINE const Swapchain& GetSwapchain() const { return m_Swapchain; }

    NODISCARD FORCEINLINE uint32 GetSwapchainImageCount() const { return (uint32)m_Swapchain.Images.size(); }
    NODISCARD FORCEINLINE const std::vector<VkImage>& GetSwapchainImages() const { return m_Swapchain.Images; }
    NODISCARD FORCEINLINE VkImage GetSwapchainImage(uint32 imageIndex) const
    {
        SE_CHECK(imageIndex < m_Swapchain.Images.size());
        return m_Swapchain.Images[imageIndex];
    }

private:
    VkPresentModeKHR FindSwapchainPresentMode() const;

private:
    RenderingSurfaceInfo m_Info;
    VkSurfaceKHR m_Surface;
    Swapchain m_Swapchain;
};

}
