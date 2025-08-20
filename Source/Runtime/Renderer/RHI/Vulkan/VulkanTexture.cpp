// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCommandList.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanMemory.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// TEXTURE UTILITY FUNCTIONS. ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

inline VkFilter TextureFilterToVulkan(TextureFilter filter)
{
    switch (filter)
    {
        case TextureFilter::Linear:  return VK_FILTER_LINEAR;
        case TextureFilter::Nearest: return VK_FILTER_NEAREST;
    }

    SE_ASSERT_NOT_REACHED;
    return VK_FILTER_NEAREST;
}

inline VkSamplerAddressMode TextureAddressModeToVulkan(TextureAddressMode addressMode)
{
    switch (addressMode)
    {
        case TextureAddressMode::Repeat:              return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case TextureAddressMode::ClampToEdge:         return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case TextureAddressMode::MirroredRepeat:      return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case TextureAddressMode::MirroredClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        case TextureAddressMode::ClampToBorder:       return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    }

    SE_ASSERT_NOT_REACHED;
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// VULKAN STORAGE TEXTURE 2D. ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

VulkanStorageTexture2D::VulkanStorageTexture2D(const Texture2DInfo& info)
    : VulkanTexture2D(GetStaticType())
    , m_TextureMemory(VK_NULL_HANDLE)
{
    CreateImageAndAllocateMemory(info);
    CreateImageView();
    CreateSampler(info);

    if (info.InitialData.HasElements())
    {
        // Immediately upload data to the GPU image storage buffer.
        SyncUploadData(info.InitialData);
    }
}

VulkanStorageTexture2D::~VulkanStorageTexture2D()
{
    // Destroy the sampler.
    vkDestroySampler(g_VulkanDriver->GetDevice(), m_Sampler.Handle, nullptr);
    m_Sampler.Handle = VK_NULL_HANDLE;
    m_Sampler = {};

    // Destroy the image view.
    vkDestroyImageView(g_VulkanDriver->GetDevice(), m_Handle.View, nullptr);
    m_Handle.View = VK_NULL_HANDLE;

    // Destroy the image.
    vkDestroyImage(g_VulkanDriver->GetDevice(), m_Handle.Image, nullptr);
    m_Handle.Image = VK_NULL_HANDLE;

    // Destroy the memory allocated for the image storage.
    vkFreeMemory(g_VulkanDriver->GetDevice(), m_TextureMemory, nullptr);
    m_TextureMemory = VK_NULL_HANDLE;
}

void VulkanStorageTexture2D::CreateImageAndAllocateMemory(const Texture2DInfo& info)
{
    // Fill the properties structure.
    m_Properties.Format = info.Format;
    m_Properties.Flags = info.Flags;
    m_Properties.SizeX = info.SizeX;
    m_Properties.SizeY = info.SizeY;

    // Create the image.
    {
        // Set the image usage flags.
        VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        if (m_Properties.Flags & TEXTURE_FLAG_SHADER_RESOURCE)
            imageUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;

        if (m_Properties.Flags & TEXTURE_FLAG_RENDER_TARGET)
        {
            if (IsTextureDepthFormat(m_Properties.Format))
                imageUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }

        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = TextureFormatToVulkan(m_Properties.Format);
        imageCreateInfo.extent.width = m_Properties.SizeX;
        imageCreateInfo.extent.height = m_Properties.SizeY;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = imageUsage;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        SE_VULKAN_CHECK(vkCreateImage(g_VulkanDriver->GetDevice(), &imageCreateInfo, nullptr, &m_Handle.Image));
    }

    // Allocate memory for the image storage buffer and bind it.
    {
        VkMemoryRequirements imageMemoryRequirements = {};
        vkGetImageMemoryRequirements(g_VulkanDriver->GetDevice(), m_Handle.Image, &imageMemoryRequirements);
        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndexForAllocation(
            imageMemoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT // The memory allocated for storing the image is located on the GPU RAM, which offers the best performance.
        );

        SE_VULKAN_CHECK(vkAllocateMemory(g_VulkanDriver->GetDevice(), &memoryAllocateInfo, nullptr, &m_TextureMemory));
        SE_VULKAN_CHECK(vkBindImageMemory(g_VulkanDriver->GetDevice(), m_Handle.Image, m_TextureMemory, 0));
    }
}

void VulkanStorageTexture2D::CreateImageView()
{
    // Set the image aspect flags.
    VkImageAspectFlags imageAspect = 0;
    if (IsTextureDepthFormat(m_Properties.Format))
        imageAspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
    else
        imageAspect |= VK_IMAGE_ASPECT_COLOR_BIT;

    // Fill the image view create info structure.
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = m_Handle.Image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = TextureFormatToVulkan(m_Properties.Format);
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    imageViewCreateInfo.subresourceRange.aspectMask = imageAspect;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    // Create the image view.
    SE_VULKAN_CHECK(vkCreateImageView(g_VulkanDriver->GetDevice(), &imageViewCreateInfo, nullptr, &m_Handle.View));
}

void VulkanStorageTexture2D::CreateSampler(const Texture2DInfo& info)
{
    // Set the sampler parameters.
    m_Sampler.MinFilter = info.MinFilter;
    m_Sampler.MagFilter = info.MagFilter;
    m_Sampler.AddressModeU = info.AddressModeU;
    m_Sampler.AddressModeV = info.AddressModeV;

    // Fill the sampler create info structure.
    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = TextureFilterToVulkan(m_Sampler.MagFilter);
    samplerCreateInfo.minFilter = TextureFilterToVulkan(m_Sampler.MinFilter);
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU = TextureAddressModeToVulkan(m_Sampler.AddressModeU);
    samplerCreateInfo.addressModeV = TextureAddressModeToVulkan(m_Sampler.AddressModeV);
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.mipLodBias = 0.0F;
    samplerCreateInfo.anisotropyEnable = VK_FALSE;
    samplerCreateInfo.maxAnisotropy = 0.0F;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = 0.0F;
    samplerCreateInfo.maxLod = 0.0F;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

    // Create the image sampler.
    SE_VULKAN_CHECK(vkCreateSampler(g_VulkanDriver->GetDevice(), &samplerCreateInfo, nullptr, &m_Sampler.Handle));
}

void VulkanStorageTexture2D::SyncUploadData(ConstVectorView<uint8> textureData)
{
    // Create the staging buffer.
    VulkanBuffer stagingBuffer;
    stagingBuffer.Invalidate(
        textureData.ByteCount(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Upload the texture data to the staging buffer.
    void* stagingBufferMappedData = stagingBuffer.Map(0, textureData.ByteCount());
    MemoryCopy(stagingBufferMappedData, textureData.Bytes(), textureData.ByteCount());
    stagingBuffer.Unmap();

    auto commandList = g_VulkanDriver->CreateCommandList(CommandListInfo()
        .SetFamily(CommandListFamily::Graphics)
    ).As<VulkanCommandList>();

    // Copy the buffer to the image storage.
    commandList->Begin();
    commandList->TransitionTexture(TransitionTextureInfo()
        .SetTexture(AdoptRef(this))
        /////////// Source. ///////////
        .SetOldLayout(TextureLayout::Undefined)
        .SetSrcPipelineStages(PIPELINE_STAGE_TOP_OF_PIPE_BIT)
        .SetSrcAccessFlags(ACCESS_FLAG_NONE_BIT)
        /////////// Destination. ///////////
        .SetNewLayout(TextureLayout::TransferDstOptimal)
        .SetDstPipelineStages(PIPELINE_STAGE_TRANSFER_BIT)
        .SetDstAccessFlags(ACCESS_FLAG_TRANSFER_WRITE_BIT)
    );
    commandList->CopyBufferToImage(AdoptRef(this), stagingBuffer.GetHandle());
    commandList->TransitionTexture(TransitionTextureInfo()
        .SetTexture(AdoptRef(this))
        /////////// Source. ///////////
        .SetOldLayout(TextureLayout::TransferDstOptimal)
        .SetSrcPipelineStages(PIPELINE_STAGE_TRANSFER_BIT)
        .SetSrcAccessFlags(ACCESS_FLAG_TRANSFER_WRITE_BIT)
        /////////// Destination. ///////////
        .SetNewLayout(TextureLayout::ShaderReadOnlyOptimal)
        .SetDstPipelineStages(PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
        .SetDstAccessFlags(ACCESS_FLAG_SHADER_READ_BIT)
    );

    // End the command list, submit it and wait for the execution to finish.
    commandList->End();
    g_VulkanDriver->ExecuteCommandListAndWait(commandList, CommandListExecuteInfo());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// VULKAN SWAPCHAIN TEXTURE 2D. ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

VulkanSwapchainTexture2D::VulkanSwapchainTexture2D(const RefPtr<VulkanSwapchain>& swapchain, uint32 imageIndex)
    : VulkanTexture2D(GetStaticType())
    , m_Swapchain(swapchain)
    , m_ImageIndex(imageIndex)
{
    SE_ASSERT(imageIndex < m_Swapchain->GetImageCount());
    m_Handle.Image = m_Swapchain->GetImages()[imageIndex];
    m_Handle.View = m_Swapchain->GetImageViews()[imageIndex];

    m_Properties.Format = TextureFormatFromVulkan(m_Swapchain->GetImmutableProperties().Format);
    m_Properties.Flags = TEXTURE_FLAG_RENDER_TARGET;
    m_Properties.SizeX = m_Swapchain->GetSizeX();
    m_Properties.SizeY = m_Swapchain->GetSizeY();
}

VulkanSwapchainTexture2D::~VulkanSwapchainTexture2D()
{
    m_Swapchain.Release();
    m_ImageIndex = 0;
}

}
