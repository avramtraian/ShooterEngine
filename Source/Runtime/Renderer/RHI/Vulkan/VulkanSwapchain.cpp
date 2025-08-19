// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanSwapchain.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

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

VulkanSwapchain::VulkanSwapchain(Window* window, uint32 imageCount, const VulkanSwapchain* oldSwapchain)
    : m_SizeX(0)
    , m_SizeY(0)
{
#if SE_PLATFORM_WIN64
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    surfaceCreateInfo.hwnd = (HWND)window->GetNativeHandle();

    if (VkResult result = vkCreateWin32SurfaceKHR(g_VulkanDriver->GetInstance(), &surfaceCreateInfo, nullptr, &m_Handle.Surface); result != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the [Vulkan] surface! (Result: %d)", result);
        return;
    }
#endif // SE_PLATFORM_WIN64

    // Set the swapchain dimensions.
    m_SizeX = window->GetSizeX();
    m_SizeY = window->GetSizeY();

    if (oldSwapchain == nullptr)
    {
        m_ImmutableProperties.Format = VK_FORMAT_B8G8R8A8_UNORM;
        m_ImmutableProperties.ColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        m_ImmutableProperties.PresentMode = FindBestPresentMode();
    }
    else
    {
        // Match the immutable properties of the old swapchain.
        m_ImmutableProperties = oldSwapchain->GetImmutableProperties();
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = m_Handle.Surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = m_ImmutableProperties.Format;
    swapchainCreateInfo.imageColorSpace = m_ImmutableProperties.ColorSpace;
    swapchainCreateInfo.imageExtent.width = m_SizeX;
    swapchainCreateInfo.imageExtent.height = m_SizeY;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = m_ImmutableProperties.PresentMode;
    swapchainCreateInfo.clipped = VK_FALSE;
    swapchainCreateInfo.oldSwapchain =
        (oldSwapchain != nullptr)
            ? oldSwapchain->GetHandle().Swapchain
            : VK_NULL_HANDLE;

    const VkResult swapchainCreateResult = vkCreateSwapchainKHR(g_VulkanDriver->GetDevice(), &swapchainCreateInfo, nullptr, &m_Handle.Swapchain);
    if (swapchainCreateResult != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the [Vulkan] swapchain! (Result: %d)", swapchainCreateResult);
        return;
    }

    // Query the images from the swapchain object.
    uint32 swapchainImageCount = 0;
    SE_VULKAN_CHECK(vkGetSwapchainImagesKHR(g_VulkanDriver->GetDevice(), m_Handle.Swapchain, &swapchainImageCount, nullptr));
    m_Images.resize(swapchainImageCount);
    SE_VULKAN_CHECK(vkGetSwapchainImagesKHR(g_VulkanDriver->GetDevice(), m_Handle.Swapchain, &swapchainImageCount, m_Images.data()));

    // Create views for each swapchain image.
    m_ImageViews.reserve(swapchainImageCount);
    for (VkImage image : m_Images)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_ImmutableProperties.Format;
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
            return;
        }
        m_ImageViews.push_back(imageViewHandle);
    }

    /* Submit information about the swapchain creation to the logger. */
    SE_LOG_TRACE("The [Vulkan] swapchain was created with the following parameters:");
    SE_LOG_TRACE("  Image count:  %d", m_Images.size())
    SE_LOG_TRACE("  Format:       %s", VulkanFormatToString(m_ImmutableProperties.Format).c_str());
    SE_LOG_TRACE("  Present mode: %s", VulkanPresentModeToString(m_ImmutableProperties.PresentMode).c_str());
}

VulkanSwapchain::~VulkanSwapchain()
{
    // Destroy the swapchain image views. Note that the images are created and managed by
    // the swapchain, and thus it is not the swapchain responsability to destroy them.
    for (VkImageView imageView : m_ImageViews)
        vkDestroyImageView(g_VulkanDriver->GetDevice(), imageView, nullptr);
    m_ImageViews.clear();
    m_Images.clear();

    // Destroy the swapchain.
    vkDestroySwapchainKHR(g_VulkanDriver->GetDevice(), m_Handle.Swapchain, nullptr);
    m_Handle.Swapchain = VK_NULL_HANDLE;

    // Destroy the surface.
    vkDestroySurfaceKHR(g_VulkanDriver->GetInstance(), m_Handle.Surface, nullptr);
    m_Handle.Surface = VK_NULL_HANDLE;
}

VkPresentModeKHR VulkanSwapchain::FindBestPresentMode() const
{
    uint32 availablePresentModeCount = 0;
    std::vector<VkPresentModeKHR> availablePresentModes;
    const VkResult getPresentModesResult = vkGetPhysicalDeviceSurfacePresentModesKHR(
        g_VulkanDriver->GetPhysicalDevice().Handle,
        m_Handle.Surface,
        &availablePresentModeCount,
        nullptr);
    if (getPresentModesResult == VK_SUCCESS)
    {
        availablePresentModes.resize(availablePresentModeCount);
        SE_VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            g_VulkanDriver->GetPhysicalDevice().Handle,
            m_Handle.Surface,
            &availablePresentModeCount,
            availablePresentModes.data()));
        SE_ENSURE(availablePresentModes.size() == availablePresentModeCount);
    }

    // List of desirable present modes. The first element in this list that is
    // available on the current platform and device will be picked.
    const std::vector<VkPresentModeKHR> desiredPresentModes =
    {
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_FIFO_KHR,
    };

    // NOTE(Traian): The Vulkan specification states that this present mode is
    // guaranteed to be available on any device and any platform.
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

}
