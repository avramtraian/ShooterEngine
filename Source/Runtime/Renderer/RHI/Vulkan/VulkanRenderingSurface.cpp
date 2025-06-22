// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Window.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.h>

namespace SE
{

VulkanRenderingSurface::VulkanRenderingSurface(const RenderingSurfaceInfo& info)
    : m_Info(info)
    , m_Surface(VK_NULL_HANDLE)
{
    Invalidate();
}

VulkanRenderingSurface::~VulkanRenderingSurface()
{
    Destroy();
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
    Destroy();

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

    /* Submit information about the swapchain creation to the logger. */
    SE_LOG_INFO("The [Vulkan] swapchain was created with the following parameters:");
    SE_LOG_INFO("  Format:       %s", VulkanFormatToString(m_Swapchain.Format).c_str());
    SE_LOG_INFO("  Present mode: %s", VulkanPresentModeToString(m_Swapchain.PresentMode).c_str());

    return true;
}

void VulkanRenderingSurface::Destroy()
{
    /* Destroy the swapchain. */
    vkDestroySwapchainKHR(g_VulkanDriver->GetDevice(), m_Swapchain.Handle, nullptr);
    m_Swapchain = {};

    /* Destroy the surface. */
    vkDestroySurfaceKHR(g_VulkanDriver->GetInstance(), m_Surface, nullptr);
    m_Surface = VK_NULL_HANDLE;
}

}
