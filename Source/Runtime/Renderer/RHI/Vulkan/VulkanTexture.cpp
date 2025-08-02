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

VulkanTexture2D::VulkanTexture2D(const Texture2DInfo& info)
    : m_IsOwnedBySwapchain(false)
    , m_Format(info.Format)
    , m_Flags(info.Flags)
    , m_SizeX(0)
    , m_SizeY(0)
{
    Invalidate(info.SizeX, info.SizeY, info.InitialData);

    // NOTE(Traian): Create the image sampler, only if the texture is marked as a shader resource.
    // As the sampler doesn't need to be recreated every time the texture is invalidated,
    // the following section is not part of the 'Invalidate' function.
    if (m_Flags & TEXTURE_FLAG_SHADER_RESOURCE)
    {
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
}

VulkanTexture2D::VulkanTexture2D(VulkanRenderingSurface& owningSurface, uint32 imageIndex)
    : m_IsOwnedBySwapchain(true)
    , m_Format(TextureFormat::Unknown)
    , m_Flags(TEXTURE_FLAG_RENDER_TARGET)
    , m_SizeX(0)
    , m_SizeY(0)
{
    InvalidateFromSurface(owningSurface, imageIndex);
}

VulkanTexture2D::~VulkanTexture2D()
{
    if (!m_IsOwnedBySwapchain)
    {
        // Destroy the texture image.
        Destroy();

        // Destroy the sampler.
        vkDestroySampler(g_VulkanDriver->GetDevice(), m_Sampler.Handle, nullptr);
        m_Sampler.Handle = VK_NULL_HANDLE;
        m_Sampler = {};
    }
    else
    {
        DestroyFromSurface();
        SE_ENSURE(m_Sampler.Handle == VK_NULL_HANDLE);
    }
    
    m_Format = TextureFormat::Unknown;
    m_Flags = TEXTURE_FLAG_NONE;
}

void VulkanTexture2D::Invalidate(uint32 sizeX, uint32 sizeY, ConstVectorView<uint8> initialData)
{
    SE_ENSURE(!m_IsOwnedBySwapchain);
    Destroy();

    m_SizeX = sizeX;
    m_SizeY = sizeY;

    // Create the image.
    {
        // Set the image usage flags.
        VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if (m_Flags & TEXTURE_FLAG_SHADER_RESOURCE)
            imageUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (m_Flags & TEXTURE_FLAG_RENDER_TARGET)
            imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = TextureFormatToVulkan(m_Format);
        imageCreateInfo.extent.width = m_SizeX;
        imageCreateInfo.extent.height = m_SizeY;
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

        SE_VULKAN_CHECK(vkAllocateMemory(g_VulkanDriver->GetDevice(), &memoryAllocateInfo, nullptr, &m_Handle.Memory));
        SE_VULKAN_CHECK(vkBindImageMemory(g_VulkanDriver->GetDevice(), m_Handle.Image, m_Handle.Memory, 0));
    }

    // Create the image view.
    {
        // Set the image aspect flags.
        VkImageAspectFlags imageAspect = 0;
        if (IsTextureDepthFormat(m_Format))
            imageAspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
        else
            imageAspect |= VK_IMAGE_ASPECT_COLOR_BIT;

        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = m_Handle.Image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = TextureFormatToVulkan(m_Format);
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        imageViewCreateInfo.subresourceRange.aspectMask = imageAspect;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        SE_VULKAN_CHECK(vkCreateImageView(g_VulkanDriver->GetDevice(), &imageViewCreateInfo, nullptr, &m_Handle.View));
    }

    // If the provided initial data buffer is not empty, upload its contents to the GPU.
    if (initialData.HasElements())
    {
        UploadTextureData(initialData);
    }
}

void VulkanTexture2D::UploadTextureData(ConstVectorView<uint8> initialData)
{
    SE_ENSURE(!m_IsOwnedBySwapchain);

    // Create the staging buffer.
    VulkanBuffer stagingBuffer;
    stagingBuffer.Invalidate(
        initialData.ByteCount(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Upload the texture data to the staging buffer.
    void* stagingBufferMappedData = stagingBuffer.Map(0, initialData.ByteCount());
    MemoryCopy(stagingBufferMappedData, initialData.Bytes(), initialData.ByteCount());
    stagingBuffer.Unmap();

    auto commandList = g_VulkanDriver->CreateCommandList(CommandListInfo().SetFamily(CommandListFamily::Graphics)).As<VulkanCommandList>();

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

void VulkanTexture2D::Destroy()
{
    SE_ENSURE(!m_IsOwnedBySwapchain);

    // Destroy the image view.
    vkDestroyImageView(g_VulkanDriver->GetDevice(), m_Handle.View, nullptr);
    m_Handle.View = VK_NULL_HANDLE;

    // Destroy the image.
    vkDestroyImage(g_VulkanDriver->GetDevice(), m_Handle.Image, nullptr);
    m_Handle.Image = VK_NULL_HANDLE;

    // Destroy the memory allocated for the image storage.
    vkFreeMemory(g_VulkanDriver->GetDevice(), m_Handle.Memory, nullptr);
    m_Handle.Memory = VK_NULL_HANDLE;

    m_SizeX = 0;
    m_SizeY = 0;
}

void VulkanTexture2D::InvalidateFromSurface(VulkanRenderingSurface& owningSurface, uint32 imageIndex)
{
    SE_ENSURE(m_IsOwnedBySwapchain);
    DestroyFromSurface();

    SE_ENSURE(imageIndex < owningSurface.GetSwapchainImageCount());
    m_Handle.Image = owningSurface.GetSwapchainImage(imageIndex);
    m_Handle.View = owningSurface.GetSwapchainImageView(imageIndex);

    m_Format = TextureFormatFromVulkan(owningSurface.GetSwapchain().Format);
    m_SizeX = owningSurface.GetSwapchain().SizeX;
    m_SizeY = owningSurface.GetSwapchain().SizeY;
}

void VulkanTexture2D::DestroyFromSurface()
{
    m_Handle = {};
    m_SizeX = 0;
    m_SizeY = 0;
}

}
