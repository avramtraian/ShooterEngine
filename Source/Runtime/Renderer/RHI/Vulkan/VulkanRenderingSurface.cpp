// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Window.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// VULKAN SURFACE DECLARATION. //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VulkanSurface::VulkanSurface(const RefPtr<Window>& targetWindow)
    : m_Handle(VK_NULL_HANDLE)
    , m_TargetWindow(targetWindow)
{
#if SE_PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    surfaceCreateInfo.hwnd = (HWND)m_TargetWindow->GetNativeHandle();

    if (VkResult result = vkCreateWin32SurfaceKHR(g_VulkanDriver->GetInstance(), &surfaceCreateInfo, nullptr, &m_Handle); result != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the [Vulkan] surface! (Result: %d)", result);
        return;
    }
#endif // SE_PLATFORM_WINDOWS
}

VulkanSurface::~VulkanSurface()
{
    vkDestroySurfaceKHR(g_VulkanDriver->GetInstance(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;
    m_TargetWindow.Release();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// VULKAN RENDERING SURFACE DECLARATION. /////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VulkanRenderingSurface::VulkanRenderingSurface(const RenderingSurfaceInfo& info)
    : m_Surface(VK_NULL_HANDLE)
    , m_SwapchainMinImageCount(info.SwapchainMinImageCount)
    , m_MaxFramesInFlight(info.MaxFramesInFlight)
    , m_CurrentFrameIndex(0)
    , m_CurrentSwapchainImageIndex(0)
{
    // Create the surface.
    m_Surface = CreateRef<VulkanSurface>(info.TargetWindow);

    // Create the swapchain.
    Invalidate();

    // Create synchronization objects.
    m_RenderFinishedFences.EnsureCapacity(m_MaxFramesInFlight);
    for (uint32 frameIndex = 0; frameIndex < m_MaxFramesInFlight; ++frameIndex)
    {
        FenceHandle fence = g_VulkanDriver->AcquireFence();
        m_RenderFinishedFences.Add(fence);
    }
}

VulkanRenderingSurface::~VulkanRenderingSurface()
{
    // Destroy synchronization objects.
    for (FenceHandle fence : m_RenderFinishedFences)
        g_VulkanDriver->RetireFence(fence);
    m_RenderFinishedFences.ClearAndShrink();

    // Release the swapchain root references.
    m_SwapchainTextures.ClearAndShrink();

    // Release the swapchain root reference.
    m_Swapchain.Release();
}

bool VulkanRenderingSurface::Invalidate()
{
    // Create the swapchain.
    m_Swapchain = CreateRef<VulkanSwapchain>(VulkanSwapchainInfo()
        .SetSurface(m_Surface)
        .SetMinImageCount(m_SwapchainMinImageCount)
        .SetMaxFramesInFlight(m_MaxFramesInFlight)
        .SetOldSwapchain(m_Swapchain)
    );

    // Create the swapchain textures.
    m_SwapchainTextures.Clear();
    m_SwapchainTextures.EnsureCapacity(m_Swapchain->GetImageCount());
    for (uint32 imageIndex = 0; imageIndex < m_Swapchain->GetImageCount(); ++imageIndex)
    {
        auto swapchainTexture = CreateRef<VulkanSwapchainTexture2D>(m_Swapchain, imageIndex);
        m_SwapchainTextures.Add(std::move(swapchainTexture));
    }

    return true;
}

RefPtr<Texture2D> VulkanRenderingSurface::GetSurfaceTexture2D(uint32 imageIndex)
{
    SE_ENSURE(imageIndex < m_SwapchainTextures.Count());
    return m_SwapchainTextures[imageIndex];
}

void VulkanRenderingSurface::BeginFrame()
{
    if (m_Surface->GetSizeX() == 0 || m_Surface->GetSizeY() == 0)
    {
        SE_LOG_ERROR("Trying to begin a frame on a rendering surface that is zero-sized. Rendering should be skipped!");
        return;
    }

    // Wait for the previous frame with the same index as this one to end.
    VkFence renderFinishedFence = (VkFence)GetRenderFinishedFence();
    g_VulkanDriver->WaitForFence(renderFinishedFence, UINT64_MAX);
    g_VulkanDriver->ResetFence(renderFinishedFence);

    // Acquire the swapchain image.
    SE_VULKAN_CHECK(vkAcquireNextImageKHR(
        g_VulkanDriver->GetDevice(),
        m_Swapchain->GetHandle(),
        UINT64_MAX,
        (VkSemaphore)GetImageAvailableSemaphore(),
        VK_NULL_HANDLE,
        &m_CurrentSwapchainImageIndex));
}

void VulkanRenderingSurface::EndFrame(bool waitForRenderFinishedSemaphore)
{
    VkSemaphore renderFinishedSemaphore = (VkSemaphore)GetRenderFinishedSemaphore();
    VkSwapchainKHR swapchainHandle = m_Swapchain->GetHandle();

    VkResult presentResult = VK_SUCCESS;
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = waitForRenderFinishedSemaphore ? 1 : 0;
    presentInfo.pWaitSemaphores = waitForRenderFinishedSemaphore ? &renderFinishedSemaphore : 0;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchainHandle;
    presentInfo.pImageIndices = &m_CurrentSwapchainImageIndex;
    presentInfo.pResults = &presentResult;

    // Submit the presentation request to the present queue.
    const VkResult queuePresentResult = vkQueuePresentKHR(g_VulkanDriver->GetPresentQueue(), &presentInfo);
    if (presentResult != VK_SUCCESS || queuePresentResult != VK_SUCCESS)
    {
        SE_LOG_WARN("vkQueuePresentKHR returned the following two results: %d, %d", presentResult, queuePresentResult);
    }

    // Increment the current frame index.
    SE_ENSURE(m_MaxFramesInFlight > 0);
    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFramesInFlight;
}

//
// NOTE(Traian): The concept of multiple frames in-flight uses two different type of indices: image indices and frame indices.
// The image index is determined in the 'BeginFrame' function and is queried by 'vkAcquireNextImageKHR'. The indices are not
// returned in any order and their selection is entirely determined by the driver! Frame indices however are managed entirely
// by the rendering surface, and each new frame index is determined by incrementing the previous one and modulo-ing it by
// the max number of frames in-flight. Thus, choosing which semaphore or fence to use in a specific frame boils down to which index
// (image or frame) to chose.
//
// 'vkAcquireNextImageKHR' requires a image-available semaphore. Since the image index is not known yet, it is obvious that we will
// index into the image-available semaphores array using the frame index. But in order to ensure that the semaphore is not still in use
// by a previous in-flight frame, we need a fence that will block the CPU (the render-finished fence). Again, since the image index is
// unknown, we will index into the render-finished fences array using the frame index.
//
// The render-finished semaphore is a bit more special because it is used (waiting for) by 'vkQueuePresentKHR'. Depending on many factors,
// such as the present mode, the presentation might happens many frames later, and thus the semaphore is locked by the 'vkQueuePresentKHR'
// by a number of frames potentially greater than the max frames in-flight. If we index by the image index, since the image was just acquired,
// we can be certain that it was already presented (At some point) and thus the corresponding semaphore is unused (since it must have been signaled,
// as no presentation would have happen otherwise). This makes indexing by the image index _always_ safe.
//

SemaphoreHandle VulkanRenderingSurface::GetImageAvailableSemaphore()
{
    return m_Swapchain->GetImageAvailableSemaphore(m_CurrentFrameIndex);
}

SemaphoreHandle VulkanRenderingSurface::GetRenderFinishedSemaphore()
{
    return m_Swapchain->GetRenderFinishedSemaphore(m_CurrentSwapchainImageIndex);
}

FenceHandle VulkanRenderingSurface::GetRenderFinishedFence()
{
    return m_RenderFinishedFences[m_CurrentFrameIndex];
}

}
