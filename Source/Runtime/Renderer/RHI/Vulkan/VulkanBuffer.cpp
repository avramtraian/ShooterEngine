// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{
    
inline uint32 FindMemoryTypeIndexForBufferAllocation(uint32 memoryTypeBits, VkMemoryPropertyFlags flags)
{
    const VkPhysicalDeviceMemoryProperties& memoryProperties = g_VulkanDriver->GetPhysicalDevice().MemoryProperties;
    for (uint32 memoryTypeIndex = 0; memoryTypeIndex < memoryProperties.memoryTypeCount; ++memoryTypeIndex)
    {
        /* First, check if the index is compatible with the required memory type index.
         * Second ,check if the memory type index has the required flags. */
        if ((memoryTypeBits & (1 << memoryTypeIndex)) &&
            ((memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & flags) == flags))
        {
            return memoryTypeIndex;
        }
    }

    SE_ASSERT(!"No memory type was found for the specified configuration!");
    return (uint32)(-1);
}

VulkanVertexBuffer::VulkanVertexBuffer(const VertexBufferInfo& info)
    : m_Handle(VK_NULL_HANDLE)
    , m_MemoryHandle(VK_NULL_HANDLE)
    , m_BufferSize(info.BufferSize)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = m_BufferSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    /* Create the buffer. */
    SE_VULKAN_CHECK(vkCreateBuffer(g_VulkanDriver->GetDevice(), &bufferCreateInfo, nullptr, &m_Handle));

    VkMemoryRequirements bufferMemoryRequirements = {};
    vkGetBufferMemoryRequirements(g_VulkanDriver->GetDevice(), m_Handle, &bufferMemoryRequirements);
    const uint32 memoryTypeIndex = FindMemoryTypeIndexForBufferAllocation(
        bufferMemoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | /* Allows the buffer memory block to be mapped to CPU space. */
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT  /* Automatic management of "availability and visibility on the host". */
    );

    VkMemoryAllocateInfo bufferAllocateInfo = {};
    bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    bufferAllocateInfo.allocationSize = bufferMemoryRequirements.size;
    bufferAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    /* Allocate a memory block for the buffer and bind them. */
    SE_VULKAN_CHECK(vkAllocateMemory(g_VulkanDriver->GetDevice(), &bufferAllocateInfo, nullptr, &m_MemoryHandle));
    SE_VULKAN_CHECK(vkBindBufferMemory(g_VulkanDriver->GetDevice(), m_Handle, m_MemoryHandle, 0));

    if (info.InitialDataSize > 0)
    {
        /* Map the buffer data. */
        void* mappedBufferData = nullptr;
        SE_VULKAN_CHECK(vkMapMemory(g_VulkanDriver->GetDevice(), m_MemoryHandle, 0, info.InitialDataSize, 0, &mappedBufferData));
        /* Copy the initial data to the mapped buffer data. */
        MemoryCopy(mappedBufferData, info.InitialData, info.InitialDataSize);
        /* Unmap the buffer data. */
        mappedBufferData = nullptr;
        vkUnmapMemory(g_VulkanDriver->GetDevice(), m_MemoryHandle);
    }
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    /* Destroy the buffer. */
    vkDestroyBuffer(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;

    /* Destroy the memory allocated for the buffer storage. */
    vkFreeMemory(g_VulkanDriver->GetDevice(), m_MemoryHandle, nullptr);
    m_MemoryHandle = VK_NULL_HANDLE;
}
   
static usize GetIndexDataTypeByteCount(IndexBufferDataType dataType)
{
    switch (dataType)
    {
        case IndexBufferDataType::UInt16: return 2;
        case IndexBufferDataType::UInt32: return 4;
    }

    SE_ASSERT(!"Invalid IndexBufferDataType!");
    return 0;
}

VulkanIndexBuffer::VulkanIndexBuffer(const IndexBufferInfo& info)
    : m_Handle(VK_NULL_HANDLE)
    , m_MemoryHandle(VK_NULL_HANDLE)
    , m_DataType(info.DataType)
    , m_IndexCount(info.IndexCount)
{
    const usize bufferSize = (usize)m_DataType * GetIndexDataTypeByteCount(m_DataType);

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    /* Create the buffer. */
    SE_VULKAN_CHECK(vkCreateBuffer(g_VulkanDriver->GetDevice(), &bufferCreateInfo, nullptr, &m_Handle));

    VkMemoryRequirements bufferMemoryRequirements = {};
    vkGetBufferMemoryRequirements(g_VulkanDriver->GetDevice(), m_Handle, &bufferMemoryRequirements);
    const uint32 memoryTypeIndex = FindMemoryTypeIndexForBufferAllocation(
        bufferMemoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | /* Allows the buffer memory block to be mapped to CPU space. */
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT  /* Automatic management of "availability and visibility on the host". */
    );

    VkMemoryAllocateInfo bufferAllocateInfo = {};
    bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    bufferAllocateInfo.allocationSize = bufferMemoryRequirements.size;
    bufferAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    /* Allocate a memory block for the buffer and bind them. */
    SE_VULKAN_CHECK(vkAllocateMemory(g_VulkanDriver->GetDevice(), &bufferAllocateInfo, nullptr, &m_MemoryHandle));
    SE_VULKAN_CHECK(vkBindBufferMemory(g_VulkanDriver->GetDevice(), m_Handle, m_MemoryHandle, 0));

    if (info.InitialIndexCount > 0)
    {
        const usize initialDataSize = (usize)info.InitialIndexCount * GetIndexDataTypeByteCount(m_DataType);

        /* Map the buffer data. */
        void* mappedBufferData = nullptr;
        SE_VULKAN_CHECK(vkMapMemory(g_VulkanDriver->GetDevice(), m_MemoryHandle, 0, initialDataSize, 0, &mappedBufferData));
        /* Copy the initial data to the mapped buffer data. */
        MemoryCopy(mappedBufferData, info.InitialIndices, initialDataSize);
        /* Unmap the buffer data. */
        mappedBufferData = nullptr;
        vkUnmapMemory(g_VulkanDriver->GetDevice(), m_MemoryHandle);
    }
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
    /* Destroy the buffer. */
    vkDestroyBuffer(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;

    /* Destroy the memory allocated for the buffer storage. */
    vkFreeMemory(g_VulkanDriver->GetDevice(), m_MemoryHandle, nullptr);
    m_MemoryHandle = VK_NULL_HANDLE;
}

}
