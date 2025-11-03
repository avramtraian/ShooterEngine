// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanSwapchain.h>

namespace SE
{

MAYBE_UNUSED static String VulkanPresentModeToString(VkPresentModeKHR presentMode)
{
    switch (presentMode)
    {
#define _SE_CASE(presentModeName) \
    case presentModeName: return VIEW(#presentModeName);
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
    return VIEW("<unstringifyable>");
}

VulkanSwapchain::VulkanSwapchain(const VulkanSwapchainInfo& info)
    : m_Handle(VK_NULL_HANDLE)
    , m_Surface(info.Surface)
    , m_SizeX(info.Surface->GetSizeX())
    , m_SizeY(info.Surface->GetSizeY())
{
    if (info.OldSwapchain.IsValid())
    {
        // Match the immutable properties of the old swapchain.
        m_ImmutableProperties = info.OldSwapchain->GetImmutableProperties();
    }
    else
    {
        m_ImmutableProperties.Format      = VK_FORMAT_B8G8R8A8_UNORM;
        m_ImmutableProperties.ColorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        m_ImmutableProperties.PresentMode = FindBestPresentMode(info.Surface->GetHandle());

        // Submit information about the swapchain creation to the logger.
        SE_LOG_TRACE("The [Vulkan] swapchain will be created with the following parameters:");
        SE_LOG_TRACE("  Min Image count: %d", info.MinImageCount)
        SE_LOG_TRACE("  Format:          %s", VulkanFormatToString(m_ImmutableProperties.Format).Characters());
        SE_LOG_TRACE("  Present mode:    %s", VulkanPresentModeToString(m_ImmutableProperties.PresentMode).Characters());
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface                  = info.Surface->GetHandle();
    swapchainCreateInfo.minImageCount            = info.MinImageCount;
    swapchainCreateInfo.imageFormat              = m_ImmutableProperties.Format;
    swapchainCreateInfo.imageColorSpace          = m_ImmutableProperties.ColorSpace;
    swapchainCreateInfo.imageExtent.width        = m_SizeX;
    swapchainCreateInfo.imageExtent.height       = m_SizeY;
    swapchainCreateInfo.imageArrayLayers         = 1;
    swapchainCreateInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount    = 0;
    swapchainCreateInfo.pQueueFamilyIndices      = nullptr;
    swapchainCreateInfo.preTransform             = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode              = m_ImmutableProperties.PresentMode;
    swapchainCreateInfo.clipped                  = VK_FALSE;
    swapchainCreateInfo.oldSwapchain             = (info.OldSwapchain.IsValid()) ? info.OldSwapchain->GetHandle() : VK_NULL_HANDLE;

    const VkResult swapchainCreateResult         = vkCreateSwapchainKHR(g_VulkanDriver->GetDevice(), &swapchainCreateInfo, nullptr, &m_Handle);
    if (swapchainCreateResult != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the [Vulkan] swapchain! (Result: %d)", swapchainCreateResult);
        return;
    }

    // Query the images from the swapchain object.
    uint32 swapchainImageCount = 0;
    SE_VULKAN_CHECK(vkGetSwapchainImagesKHR(g_VulkanDriver->GetDevice(), m_Handle, &swapchainImageCount, nullptr));
    m_Images.SetCountDefaulted(swapchainImageCount);
    SE_VULKAN_CHECK(vkGetSwapchainImagesKHR(g_VulkanDriver->GetDevice(), m_Handle, &swapchainImageCount, m_Images.Elements()));

    // Create views for each swapchain image.
    m_ImageViews.EnsureCapacity(swapchainImageCount);
    for (VkImage image : m_Images)
    {
        VkImageViewCreateInfo imageViewCreateInfo           = {};
        imageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image                           = image;
        imageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format                          = m_ImmutableProperties.Format;
        imageViewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
        imageViewCreateInfo.subresourceRange.levelCount     = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount     = 1;

        VkImageView imageViewHandle                         = VK_NULL_HANDLE;
        if (VkResult result = vkCreateImageView(g_VulkanDriver->GetDevice(), &imageViewCreateInfo, nullptr, &imageViewHandle); result != VK_SUCCESS)
        {
            SE_LOG_ERROR("Failed to create [Vulkan] image view for swapchain image! (Result: %d)", result);
            return;
        }
        m_ImageViews.Add(imageViewHandle);
    }

    // Create the synchronization objects.
    {
        m_ImageAvailableSemaphores.EnsureCapacity(info.MaxFramesInFlight);
        m_RenderFinishedSemaphores.EnsureCapacity(info.MaxFramesInFlight);

        for (uint32 frameIndex = 0; frameIndex < info.MaxFramesInFlight; ++frameIndex)
        {
            m_ImageAvailableSemaphores.Add(g_VulkanDriver->AcquireSemaphore());
            m_RenderFinishedSemaphores.Add(g_VulkanDriver->AcquireSemaphore());
        }
    }
}

VulkanSwapchain::~VulkanSwapchain()
{
    // Destroy the synchronization objects.
    {
        for (SemaphoreHandle semaphore : m_ImageAvailableSemaphores)
            g_VulkanDriver->RetireSemaphore(semaphore);
        m_ImageAvailableSemaphores.ClearAndShrink();

        for (SemaphoreHandle semaphore : m_RenderFinishedSemaphores)
            g_VulkanDriver->RetireSemaphore(semaphore);
        m_RenderFinishedSemaphores.ClearAndShrink();
    }

    // Destroy the swapchain image views. Note that the images are created and managed by
    // the swapchain, and thus it is not the swapchain responsability to destroy them.
    for (VkImageView imageView : m_ImageViews)
        vkDestroyImageView(g_VulkanDriver->GetDevice(), imageView, nullptr);
    m_ImageViews.ClearAndShrink();
    m_Images.ClearAndShrink();

    // Destroy the swapchain.
    vkDestroySwapchainKHR(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;
}

VkPresentModeKHR VulkanSwapchain::FindBestPresentMode(VkSurfaceKHR surface) const
{
    uint32                   availablePresentModeCount = 0;
    Vector<VkPresentModeKHR> availablePresentModes;
    const VkResult           getPresentModesResult =
        vkGetPhysicalDeviceSurfacePresentModesKHR(g_VulkanDriver->GetPhysicalDevice().Handle, surface, &availablePresentModeCount, nullptr);
    if (getPresentModesResult == VK_SUCCESS)
    {
        availablePresentModes.SetCountDefaulted(availablePresentModeCount);
        SE_VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(g_VulkanDriver->GetPhysicalDevice().Handle, surface, &availablePresentModeCount,
                                                                  availablePresentModes.Elements()));
        SE_ENSURE(availablePresentModes.Count() == availablePresentModeCount);
    }

    // List of desirable present modes. The first element in this list that is
    // available on the current platform and device will be picked.
    const Vector<VkPresentModeKHR> desiredPresentModes = {
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
                presentMode                = desiredPresentMode;
                desiredPresentModeWasFound = true;
                break;
            }
        }

        if (desiredPresentModeWasFound)
            break;
    }

    return presentMode;
}

} // namespace SE
