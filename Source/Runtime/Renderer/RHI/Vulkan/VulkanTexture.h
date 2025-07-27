// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/Texture.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

NODISCARD static inline VkFormat TextureFormatToVulkan(TextureFormat textureFormat)
{
    switch (textureFormat)
    {
        case TextureFormat::Unknown:  return VK_FORMAT_UNDEFINED;
        case TextureFormat::R8G8B8A8: return VK_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::R8G8B8:   return VK_FORMAT_R8G8B8_UNORM;
        case TextureFormat::B8G8R8A8: return VK_FORMAT_B8G8R8A8_UNORM;
    }

    SE_ASSERT_NOT_REACHED;
    return VK_FORMAT_UNDEFINED;
}

NODISCARD static inline TextureFormat TextureFormatFromVulkan(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_UNDEFINED:      return TextureFormat::Unknown;
        case VK_FORMAT_R8G8B8_UNORM:   return TextureFormat::R8G8B8;
        case VK_FORMAT_R8G8B8A8_UNORM: return TextureFormat::R8G8B8A8;
        case VK_FORMAT_B8G8R8A8_UNORM: return TextureFormat::B8G8R8A8;
    }

    SE_ASSERT_NOT_REACHED;
    return TextureFormat::Unknown;
}

class VulkanTexture2D : public Texture2D
{
public:
    struct Handle
    {
        VkImage Image    { VK_NULL_HANDLE };
        VkImageView View { VK_NULL_HANDLE };
    };

public:
    VulkanTexture2D(const Texture2DInfo& info);
    VulkanTexture2D(VulkanRenderingSurface& owningSurface, uint32 imageIndex);

    virtual ~VulkanTexture2D() override;

    NODISCARD FORCEINLINE virtual TextureFormat GetFormat() const override { return m_Format; }
    NODISCARD FORCEINLINE virtual TextureFlags GetFlags() const override { return m_Flags; }
    NODISCARD FORCEINLINE virtual uint32 GetSizeX() const override { return m_SizeX; }
    NODISCARD FORCEINLINE virtual uint32 GetSizeY() const override { return m_SizeY; }

public:
    NODISCARD FORCEINLINE const Handle& GetHandle() const { return m_Handle; }

    void InvalidateFromSurface(VulkanRenderingSurface& owningSurface, uint32 imageIndex);
    void DestroyFromSurface(VulkanRenderingSurface& owningSurface);

private:
    bool m_IsOwnedBySwapchain;
    Handle m_Handle;
    TextureFormat m_Format;
    TextureFlags m_Flags;
    uint32 m_SizeX;
    uint32 m_SizeY;
};

}
