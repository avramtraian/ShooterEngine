// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/Texture.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanSwapchain.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEXTURE UTILITY FUNCTIONS. ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// VULKAN GENERIC TEXTURE 2D. ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

enum class VulkanTexture2DType : uint8
{
    Unknown = 0,
    Storage,
    Swapchain,
};

class VulkanTexture2D : public Texture2D
{
public:
    struct Handle
    {
        VkImage     Image { VK_NULL_HANDLE };
        VkImageView View  { VK_NULL_HANDLE };
    };

    struct Properties
    {
        TextureFormat Format { TextureFormat::Unknown };
        TextureFlags  Flags  { TEXTURE_FLAG_NONE };
        uint32        SizeX  { 0 };
        uint32        SizeY  { 0 };
    };

public:
    VulkanTexture2D(VulkanTexture2DType type) : m_Type(type) {}
    virtual ~VulkanTexture2D() override = default;
    NODISCARD FORCEINLINE VulkanTexture2DType GetType() const { return m_Type; }

    NODISCARD FORCEINLINE const Handle& GetHandle() const { return m_Handle; }
    NODISCARD FORCEINLINE const Properties& GetProperties() const { return m_Properties; }

    NODISCARD FORCEINLINE virtual uint32 GetSizeX() const override { return m_Properties.SizeX; }
    NODISCARD FORCEINLINE virtual uint32 GetSizeY() const override { return m_Properties.SizeY; }
    NODISCARD FORCEINLINE virtual TextureFormat GetFormat() const override { return m_Properties.Format; }
    NODISCARD FORCEINLINE virtual TextureFlags GetFlags() const override { return m_Properties.Flags; }

protected:
    VulkanTexture2DType m_Type;
    Handle m_Handle;
    Properties m_Properties;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// VULKAN STORAGE TEXTURE 2D. ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class VulkanStorageTexture2D : public VulkanTexture2D
{
public:
    struct Sampler
    {
        VkSampler          Handle       { VK_NULL_HANDLE };
        TextureFilter      MinFilter    { TextureFilter::Linear };
        TextureFilter      MagFilter    { TextureFilter::Linear };
        TextureAddressMode AddressModeU { TextureAddressMode::Repeat };
        TextureAddressMode AddressModeV { TextureAddressMode::Repeat };
    };

public:
    VulkanStorageTexture2D(const Texture2DInfo& info);
    virtual ~VulkanStorageTexture2D() override;
    NODISCARD FORCEINLINE static VulkanTexture2DType GetStaticType() { return VulkanTexture2DType::Storage; }

    NODISCARD FORCEINLINE VkDeviceMemory GetTextureMemory() const { return m_TextureMemory; }
    NODISCARD FORCEINLINE const Sampler& GetSampler() const { return m_Sampler; }

private:
    void CreateImageAndAllocateMemory(const Texture2DInfo& info);
    void CreateImageView();
    void CreateSampler(const Texture2DInfo& info);
    void SyncUploadData(ConstVectorView<uint8> textureData);

private:
    VkDeviceMemory m_TextureMemory;
    Sampler m_Sampler;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// VULKAN SWAPCHAIN TEXTURE 2D. ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class VulkanSwapchainTexture2D : public VulkanTexture2D
{
public:
    VulkanSwapchainTexture2D(const RefPtr<VulkanSwapchain>& swapchain, uint32 imageIndex);
    virtual ~VulkanSwapchainTexture2D() override;
    NODISCARD FORCEINLINE static VulkanTexture2DType GetStaticType() { return VulkanTexture2DType::Swapchain; }

private:
    RefPtr<VulkanSwapchain> m_Swapchain;
    uint32 m_ImageIndex;
};

}
