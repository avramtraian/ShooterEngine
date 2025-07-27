// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Window.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

VulkanRenderingSurface::VulkanRenderingSurface(const RenderingSurfaceInfo& info)
    : m_Info(info)
    , m_Surface(VK_NULL_HANDLE)
    , m_MaxFramesInFlight(info.SwapchainImageCount)
    , m_CurrentFrameIndex(0)
    , m_CurrentSwapchainImageIndex(0)
{
    Invalidate();
}

VulkanRenderingSurface::~VulkanRenderingSurface()
{
    Destroy(true);
}

VkPresentModeKHR VulkanRenderingSurface::FindSwapchainPresentMode() const
{
    uint32 availablePresentModeCount = 0;
    std::vector<VkPresentModeKHR> availablePresentModes;
    const VkResult getPresentModesResult = vkGetPhysicalDeviceSurfacePresentModesKHR(
        g_VulkanDriver->GetPhysicalDevice().Handle,
        m_Surface,
        &availablePresentModeCount,
        nullptr);
    if (getPresentModesResult == VK_SUCCESS)
    {
        availablePresentModes.resize(availablePresentModeCount);
        SE_VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            g_VulkanDriver->GetPhysicalDevice().Handle,
            m_Surface,
            &availablePresentModeCount,
            availablePresentModes.data()));
        SE_ENSURE(availablePresentModes.size() == availablePresentModeCount);
    }

    /* List of desirable present modes. The first element in this list that is
     * available on the current platform and device will be picked. */
    const std::vector<VkPresentModeKHR> desiredPresentModes =
    {
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_FIFO_KHR,
    };

    /* NOTE(Traian): The Vulkan specification states that this present mode is
     * guaranteed to be available on any device and any platform. */
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    for (VkPresentModeKHR desiredPresentMode : desiredPresentModes)
    {
        bool desiredPresentModeWasFound = false;
        for (VkPresentModeKHR availablePresentMode : availablePresentModes)
        {
            if (desiredPresentMode == availablePresentMode)
            {
                presentMode = desiredPresentMode;
                desiredPresentModeWasFound = true;
                break;
            }
        }

        if (desiredPresentModeWasFound)
            break;
    }

    return presentMode;
}

static std::string VulkanPresentModeToString(VkPresentModeKHR presentMode)
{
    switch (presentMode)
    {
#define _SE_CASE(presentModeName) case presentModeName: return #presentModeName;
        _SE_CASE(VK_PRESENT_MODE_IMMEDIATE_KHR);
        _SE_CASE(VK_PRESENT_MODE_MAILBOX_KHR);
        _SE_CASE(VK_PRESENT_MODE_FIFO_KHR);
        _SE_CASE(VK_PRESENT_MODE_FIFO_RELAXED_KHR);
        _SE_CASE(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
        _SE_CASE(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
        _SE_CASE(VK_PRESENT_MODE_FIFO_LATEST_READY_EXT);
#undef _SE_CASE
    }

    SE_ASSERT_NOT_REACHED;
    return "<unstringifyable>";
}

bool VulkanRenderingSurface::Invalidate()
{
    /* NOTE(Traian): Invalidating the rendering surface causes the invalidation of a lot of resources, especially textures. While it is
     * definitely possible to keep track of them and not cause such a big wait period, the complexity it requires is just not worth it.
     * How many times is the window resized during a game session? And how important is for the user to not drop 2-3 frames?
     * 
     * If for whatever reason you want to refactor this function and refactor this function (and remove the 'vkDeviceWaitIdle' call), keep
     * in mind that a lot of the following code assumes that there are no resources that are in use on the GPU! */
    SE_VULKAN_CHECK(vkDeviceWaitIdle(g_VulkanDriver->GetDevice()));

    Destroy(false);

#if SE_PLATFORM_WIN64
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    surfaceCreateInfo.hwnd = (HWND)m_Info.OwningWindow->GetNativeHandle();

    if (VkResult result = vkCreateWin32SurfaceKHR(g_VulkanDriver->GetInstance(), &surfaceCreateInfo, nullptr, &m_Surface); result != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the [Vulkan] surface! (Result: %d)", result);
        return false;
    }
#endif // SE_PLATFORM_WIN64

    m_Swapchain.SizeX = m_Info.OwningWindow->GetSizeX();
    m_Swapchain.SizeY = m_Info.OwningWindow->GetSizeY();
    m_Swapchain.Format = VK_FORMAT_B8G8R8A8_UNORM;
    m_Swapchain.ColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    m_Swapchain.PresentMode = FindSwapchainPresentMode();

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = m_Surface;
    swapchainCreateInfo.minImageCount = m_Info.SwapchainImageCount;
    swapchainCreateInfo.imageFormat = m_Swapchain.Format;
    swapchainCreateInfo.imageColorSpace = m_Swapchain.ColorSpace;
    swapchainCreateInfo.imageExtent.width = m_Swapchain.SizeX;
    swapchainCreateInfo.imageExtent.height = m_Swapchain.SizeY;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = m_Swapchain.PresentMode;
    swapchainCreateInfo.clipped = VK_FALSE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    const VkResult swapchainCreateResult = vkCreateSwapchainKHR(g_VulkanDriver->GetDevice(), &swapchainCreateInfo, nullptr, &m_Swapchain.Handle);
    if (swapchainCreateResult != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the [Vulkan] swapchain! (Result: %d)", swapchainCreateResult);
        return false;
    }

    /* Query the images from the swapchain object. */
    uint32 swapchainImageCount = 0;
    SE_VULKAN_CHECK(vkGetSwapchainImagesKHR(g_VulkanDriver->GetDevice(), m_Swapchain.Handle, &swapchainImageCount, nullptr));
    m_Swapchain.Images.resize(swapchainImageCount);
    SE_VULKAN_CHECK(vkGetSwapchainImagesKHR(g_VulkanDriver->GetDevice(), m_Swapchain.Handle, &swapchainImageCount, m_Swapchain.Images.data()));

    /* Create views for each swapchain image. */
    m_Swapchain.ImageViews.reserve(swapchainImageCount);
    for (VkImage image : m_Swapchain.Images)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_Swapchain.Format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        VkImageView imageViewHandle = VK_NULL_HANDLE;
        if (VkResult result = vkCreateImageView(g_VulkanDriver->GetDevice(), &imageViewCreateInfo, nullptr, &imageViewHandle); result != VK_SUCCESS)
        {
            SE_LOG_ERROR("Failed to create [Vulkan] image view for swapchain image! (Result: %d)", result);
            return false;
        }
        m_Swapchain.ImageViews.push_back(imageViewHandle);
    }

    if (m_Swapchain.Textures.empty())
    {
        /* Create 2D textures owned by the swapchain for later access. */
        m_Swapchain.Textures.reserve(m_Swapchain.Images.size());
        for (uint32 imageIndex = 0; imageIndex < (uint32)m_Swapchain.Images.size(); ++imageIndex)
        {
            auto texture = std::make_shared<VulkanTexture2D>(*this, imageIndex);
            m_Swapchain.Textures.push_back(std::move(texture));
        }
    }
    else
    {
        for (uint32 imageIndex = 0; imageIndex < (uint32)m_Swapchain.Images.size(); ++imageIndex)
        {
            m_Swapchain.Textures[imageIndex]->InvalidateFromSurface(*this, imageIndex);
        }
    }

    /* Submit information about the swapchain creation to the logger. */
    SE_LOG_TRACE("The [Vulkan] swapchain was created with the following parameters:");
    SE_LOG_TRACE("  Image count:  %d", m_Swapchain.Images.size())
    SE_LOG_TRACE("  Format:       %s", VulkanFormatToString(m_Swapchain.Format).c_str());
    SE_LOG_TRACE("  Present mode: %s", VulkanPresentModeToString(m_Swapchain.PresentMode).c_str());

    /* Create synchronization objects. */
    {
        m_ImageAvailableSemaphores.reserve(m_MaxFramesInFlight);
        m_RenderFinishedSemaphores.reserve(m_MaxFramesInFlight);
        m_RenderFinishedFences.reserve(m_MaxFramesInFlight);

        for (uint32 frameIndex = 0; frameIndex < m_MaxFramesInFlight; ++frameIndex)
        {
            m_ImageAvailableSemaphores.push_back(g_VulkanDriver->AcquireSemaphore());
            m_RenderFinishedSemaphores.push_back(g_VulkanDriver->AcquireSemaphore());
            m_RenderFinishedFences.push_back(g_VulkanDriver->AcquireFence());
        }
    }

    return true;
}

void VulkanRenderingSurface::Destroy(bool shouldDestroyTextures)
{
    /* Destroy synchronization objects. */
    if (!m_ImageAvailableSemaphores.empty())
    {
        for (uint32 frameIndex = 0; frameIndex < m_MaxFramesInFlight; ++frameIndex)
        {
            g_VulkanDriver->RetireSemaphore(m_ImageAvailableSemaphores[frameIndex]);
            g_VulkanDriver->RetireSemaphore(m_RenderFinishedSemaphores[frameIndex]);
            g_VulkanDriver->RetireFence(m_RenderFinishedFences[frameIndex]);
        }

        m_ImageAvailableSemaphores.clear();
        m_RenderFinishedSemaphores.clear();
        m_RenderFinishedFences.clear();
    }

    /* Destroy the swapchain textures. */
    for (auto& texture : m_Swapchain.Textures)
        texture->DestroyFromSurface();

    if (shouldDestroyTextures)
        m_Swapchain.Textures.clear();

    /* Destroy swapchain image views.
     * NOTE(Traian): Images themselves are created and managed by the swapchain object. */
    for (VkImageView imageView : m_Swapchain.ImageViews)
        vkDestroyImageView(g_VulkanDriver->GetDevice(), imageView, nullptr);
    m_Swapchain.ImageViews.clear();
    m_Swapchain.Images.clear();

    /* Destroy the swapchain. */
    vkDestroySwapchainKHR(g_VulkanDriver->GetDevice(), m_Swapchain.Handle, nullptr);
    m_Swapchain = {};

    /* Destroy the surface. */
    vkDestroySurfaceKHR(g_VulkanDriver->GetInstance(), m_Surface, nullptr);
    m_Surface = VK_NULL_HANDLE;
}

std::shared_ptr<Texture2D> VulkanRenderingSurface::GetSurfaceTexture2D(uint32 imageIndex)
{
    SE_ENSURE(imageIndex < m_Swapchain.Textures.size());
    return m_Swapchain.Textures[imageIndex];
}

void VulkanRenderingSurface::BeginFrame()
{
    /* Wait for the previous frame with the same index as this one to end. */
    g_VulkanDriver->WaitForFence(m_RenderFinishedFences[m_CurrentFrameIndex], UINT64_MAX);
    g_VulkanDriver->ResetFence(m_RenderFinishedFences[m_CurrentFrameIndex]);

    /* Acquire the swapchain image. */
    SE_VULKAN_CHECK(vkAcquireNextImageKHR(
        g_VulkanDriver->GetDevice(),
        m_Swapchain.Handle,
        UINT64_MAX,
        (VkSemaphore)m_ImageAvailableSemaphores[m_CurrentFrameIndex],
        VK_NULL_HANDLE,
        &m_CurrentSwapchainImageIndex));
}

void VulkanRenderingSurface::EndFrame()
{
    /* List of semaphores that are required to be signaled before the presentation occurs. */
    VkSemaphore submitWaitSemaphores[] = {
        (VkSemaphore)m_RenderFinishedSemaphores[m_CurrentFrameIndex]
    };

    VkResult presentResult = VK_SUCCESS;
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = (uint32)SE_ARRAY_COUNT(submitWaitSemaphores);
    presentInfo.pWaitSemaphores = submitWaitSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_Swapchain.Handle;
    presentInfo.pImageIndices = &m_CurrentSwapchainImageIndex;
    presentInfo.pResults = &presentResult;

    /* Submit the presentation request to the present queue. */
    const VkResult queuePresentResult = vkQueuePresentKHR(g_VulkanDriver->GetPresentQueue(), &presentInfo);

    /* Increment the current frame index. */
    SE_ENSURE(m_MaxFramesInFlight > 0);
    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFramesInFlight;
}

}
