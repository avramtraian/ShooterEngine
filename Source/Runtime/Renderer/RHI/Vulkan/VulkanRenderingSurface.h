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
        VkSwapchainKHR           Handle      { VK_NULL_HANDLE };
        uint32                   SizeX       { 0 };
        uint32                   SizeY       { 0 };
        VkFormat                 Format      { VK_FORMAT_UNDEFINED };
        VkColorSpaceKHR          ColorSpace  { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        VkPresentModeKHR         PresentMode { VK_PRESENT_MODE_FIFO_KHR };
        std::vector<VkImage>     Images;
        std::vector<VkImageView> ImageViews;

        std::vector<std::shared_ptr<VulkanTexture2D>> Textures;
    };

public:
    VulkanRenderingSurface(const RenderingSurfaceInfo& info);
    virtual ~VulkanRenderingSurface() override;

public:
    NODISCARD FORCEINLINE virtual uint32 GetMaxFramesInFlight() const override { return m_MaxFramesInFlight; }
    NODISCARD FORCEINLINE virtual uint32 GetCurrentFrameIndex() const override { return m_CurrentFrameIndex; }
    NODISCARD FORCEINLINE virtual uint32 GetCurrentSwapchainImageIndex() const override { return m_CurrentSwapchainImageIndex; }

    NODISCARD FORCEINLINE virtual TextureFormat GetSurfaceFormat() const override { return TextureFormatFromVulkan(m_Swapchain.Format); }
    NODISCARD FORCEINLINE virtual uint32 GetSurfaceSizeX() const override { return m_Swapchain.SizeX; }
    NODISCARD FORCEINLINE virtual uint32 GetSurfaceSizeY() const override { return m_Swapchain.SizeY; }

    virtual bool Invalidate() override;
    void Destroy(bool shouldDestroyTextures);

    virtual std::shared_ptr<Texture2D> GetSurfaceTexture2D(uint32 imageIndex) override;
    virtual std::shared_ptr<Texture2D> GetCurrentSurfaceTexture2D() override { return GetSurfaceTexture2D(m_CurrentSwapchainImageIndex); }

    virtual void BeginFrame() override;
    virtual void EndFrame() override;

    FORCEINLINE virtual SemaphoreHandle GetImageAvailableSemaphore() override { return m_ImageAvailableSemaphores[m_CurrentFrameIndex]; }
    FORCEINLINE virtual SemaphoreHandle GetRenderFinishedSemaphore() override { return m_RenderFinishedSemaphores[m_CurrentFrameIndex]; }
    FORCEINLINE virtual FenceHandle GetRenderFinishedFence() override { return m_RenderFinishedFences[m_CurrentFrameIndex]; }

public:
    NODISCARD FORCEINLINE VkSurfaceKHR GetSurface() const { return m_Surface; }
    NODISCARD FORCEINLINE const Swapchain& GetSwapchain() const { return m_Swapchain; }

    NODISCARD FORCEINLINE uint32 GetSwapchainImageCount() const { return (uint32)m_Swapchain.Images.size(); }
    NODISCARD FORCEINLINE const std::vector<VkImage>& GetSwapchainImages() const { return m_Swapchain.Images; }
    NODISCARD FORCEINLINE const std::vector<VkImageView>& GetSwapchainImageViews() const { return m_Swapchain.ImageViews; }
    NODISCARD FORCEINLINE VkImage GetSwapchainImage(uint32 imageIndex) const
    {
        SE_CHECK(imageIndex < m_Swapchain.Images.size());
        return m_Swapchain.Images[imageIndex];
    }
    NODISCARD FORCEINLINE VkImageView GetSwapchainImageView(uint32 imageIndex) const
    {
        SE_CHECK(imageIndex < m_Swapchain.ImageViews.size());
        return m_Swapchain.ImageViews[imageIndex];
    }

private:
    VkPresentModeKHR FindSwapchainPresentMode() const;

private:
    RenderingSurfaceInfo m_Info;
    VkSurfaceKHR m_Surface;
    Swapchain m_Swapchain;

    uint32 m_MaxFramesInFlight;
    uint32 m_CurrentFrameIndex;
    uint32 m_CurrentSwapchainImageIndex;

    std::vector<SemaphoreHandle> m_ImageAvailableSemaphores;
    std::vector<SemaphoreHandle> m_RenderFinishedSemaphores;
    std::vector<FenceHandle> m_RenderFinishedFences;
};

}
