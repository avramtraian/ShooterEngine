// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

VulkanTexture2D::VulkanTexture2D(const Texture2DInfo& info)
    : m_Format(info.Format)
    , m_SizeX(info.SizeX)
    , m_SizeY(info.SizeY)
{}

VulkanTexture2D::VulkanTexture2D(VulkanRenderingSurface& owningSurface, uint32 imageIndex)
    : m_IsOwnedBySwapchain(true)
{
    InvalidateFromSurface(owningSurface, imageIndex);
}

VulkanTexture2D::~VulkanTexture2D()
{
    if (!m_IsOwnedBySwapchain)
    {
        /* Destroy the view and the image. */
        vkDestroyImageView(g_VulkanDriver->GetDevice(), m_Handle.View, nullptr);
        vkDestroyImage(g_VulkanDriver->GetDevice(), m_Handle.Image, nullptr);
    }
    
    m_Handle = {};
    m_SizeX = 0;
    m_SizeY = 0;
    m_Format = TextureFormat::Unknown;
}

void VulkanTexture2D::InvalidateFromSurface(VulkanRenderingSurface& owningSurface, uint32 imageIndex)
{
    SE_ENSURE(m_IsOwnedBySwapchain);
    DestroyFromSurface(owningSurface);

    SE_ENSURE(imageIndex < owningSurface.GetSwapchainImageCount());
    m_Handle.Image = owningSurface.GetSwapchainImage(imageIndex);
    m_Handle.View = owningSurface.GetSwapchainImageView(imageIndex);

    m_Format = TextureFormatFromVulkan(owningSurface.GetSwapchain().Format);
    m_SizeX = owningSurface.GetSwapchain().SizeX;
    m_SizeY = owningSurface.GetSwapchain().SizeY;
}

void VulkanTexture2D::DestroyFromSurface(VulkanRenderingSurface& owningSurface)
{
    m_Handle = {};
    m_SizeX = 0;
    m_SizeY = 0;
    m_Format = TextureFormat::Unknown;
}

}
