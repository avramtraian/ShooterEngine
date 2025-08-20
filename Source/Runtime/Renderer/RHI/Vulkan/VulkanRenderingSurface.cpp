// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Window.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

VulkanRenderingSurface::VulkanRenderingSurface(const RenderingSurfaceInfo& info)
    : m_OwningWindow(info.OwningWindow)
    , m_Surface(VK_NULL_HANDLE)
    , m_SwapchainMinImageCount(info.SwapchainMinImageCount)
    , m_MaxFramesInFlight(info.MaxFramesInFlight)
    , m_CurrentFrameIndex(0)
    , m_CurrentSwapchainImageIndex(0)
{
    // Create the window surface.
#if SE_PLATFORM_WIN64
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    surfaceCreateInfo.hwnd = (HWND)m_OwningWindow->GetNativeHandle();

    if (VkResult result = vkCreateWin32SurfaceKHR(g_VulkanDriver->GetInstance(), &surfaceCreateInfo, nullptr, &m_Surface); result != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the [Vulkan] surface! (Result: %d)", result);
        return;
    }
#endif // SE_PLATFORM_WIN64

    // Create the swapchain.
    Invalidate();

    // Create synchronization objects.
    m_RenderFinishedFences.reserve(m_MaxFramesInFlight);
    for (uint32 frameIndex = 0; frameIndex < m_MaxFramesInFlight; ++frameIndex)
    {
        FenceHandle fence = g_VulkanDriver->AcquireFence();
        m_RenderFinishedFences.push_back(fence);
    }
}

VulkanRenderingSurface::~VulkanRenderingSurface()
{
    // Destroy synchronization objects.
    for (FenceHandle fence : m_RenderFinishedFences)
        g_VulkanDriver->RetireFence(fence);
    m_RenderFinishedFences.clear();

    // Release the swapchain root references.
    m_SwapchainTextures.clear();

    // Release the swapchain root reference.
    m_Swapchain.Release();

    // Destroy the window surface.
    vkDestroySurfaceKHR(g_VulkanDriver->GetInstance(), m_Surface, nullptr);
    m_Surface = VK_NULL_HANDLE;
}

bool VulkanRenderingSurface::Invalidate()
{
    // Create the swapchain.
    m_Swapchain = CreateRef<VulkanSwapchain>(VulkanSwapchainInfo()
        .SetSurface(m_Surface)
        .SetSize(m_OwningWindow->GetSizeX(), m_OwningWindow->GetSizeY())
        .SetMinImageCount(m_SwapchainMinImageCount)
        .SetMaxFramesInFlight(m_MaxFramesInFlight)
        .SetOldSwapchain(m_Swapchain)
    );

    // Create the swapchain textures.
    m_SwapchainTextures.clear();
    m_SwapchainTextures.reserve(m_Swapchain->GetImageCount());
    for (uint32 imageIndex = 0; imageIndex < m_Swapchain->GetImageCount(); ++imageIndex)
    {
        auto swapchainTexture = CreateRef<VulkanSwapchainTexture2D>(m_Swapchain, imageIndex);
        m_SwapchainTextures.push_back(std::move(swapchainTexture));
    }

    return true;
}

RefPtr<Texture2D> VulkanRenderingSurface::GetSurfaceTexture2D(uint32 imageIndex)
{
    SE_ENSURE(imageIndex < m_SwapchainTextures.size());
    return m_SwapchainTextures[imageIndex];
}

void VulkanRenderingSurface::BeginFrame()
{
    if (m_OwningWindow->GetSizeX() == 0 || m_OwningWindow->GetSizeY() == 0)
    {
        SE_LOG_ERROR("Trying to begin a frame on a rendering surface whose parent window is zero-sized. Rendering should be skipped!");
        return;
    }

    /* Wait for the previous frame with the same index as this one to end. */
    g_VulkanDriver->WaitForFence(m_RenderFinishedFences[m_CurrentFrameIndex], UINT64_MAX);
    g_VulkanDriver->ResetFence(m_RenderFinishedFences[m_CurrentFrameIndex]);

    /* Acquire the swapchain image. */
    SE_VULKAN_CHECK(vkAcquireNextImageKHR(
        g_VulkanDriver->GetDevice(),
        m_Swapchain->GetHandle(),
        UINT64_MAX,
        (VkSemaphore)m_Swapchain->GetImageAvailableSemaphore(m_CurrentFrameIndex),
        VK_NULL_HANDLE,
        &m_CurrentSwapchainImageIndex));
}

void VulkanRenderingSurface::EndFrame()
{
    // List of semaphores that are required to be signaled before the presentation occurs.
    VkSemaphore submitWaitSemaphores[] = {
        (VkSemaphore)m_Swapchain->GetRenderFinishedSemaphore(m_CurrentFrameIndex)
    };

    // Handle of the swapchain.
    VkSwapchainKHR swapchainHandle = m_Swapchain->GetHandle();

    VkResult presentResult = VK_SUCCESS;
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = (uint32)SE_ARRAY_COUNT(submitWaitSemaphores);
    presentInfo.pWaitSemaphores = submitWaitSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchainHandle;
    presentInfo.pImageIndices = &m_CurrentSwapchainImageIndex;
    presentInfo.pResults = &presentResult;

    // Submit the presentation request to the present queue.
    const VkResult queuePresentResult = vkQueuePresentKHR(g_VulkanDriver->GetPresentQueue(), &presentInfo);

    // Increment the current frame index.
    SE_ENSURE(m_MaxFramesInFlight > 0);
    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFramesInFlight;
}

}
