// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanMemory.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

void VulkanBuffer::Invalidate(usize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags)
{
    // Release the old buffer.
    Release();

    // Set the buffer size.
    m_BufferSize = size;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = m_BufferSize;
    bufferCreateInfo.usage = usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    // Create the buffer.
    SE_VULKAN_CHECK(vkCreateBuffer(g_VulkanDriver->GetDevice(), &bufferCreateInfo, nullptr, &m_BufferHandle));

    VkMemoryRequirements bufferMemoryRequirements = {};
    vkGetBufferMemoryRequirements(g_VulkanDriver->GetDevice(), m_BufferHandle, &bufferMemoryRequirements);
    const uint32 memoryTypeIndex = FindMemoryTypeIndexForAllocation(
        bufferMemoryRequirements.memoryTypeBits,
        memoryPropertyFlags
    );

    VkMemoryAllocateInfo bufferAllocateInfo = {};
    bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    bufferAllocateInfo.allocationSize = bufferMemoryRequirements.size;
    bufferAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    // Allocate a memory block for the buffer and bind them.
    SE_VULKAN_CHECK(vkAllocateMemory(g_VulkanDriver->GetDevice(), &bufferAllocateInfo, nullptr, &m_BufferMemory));
    SE_VULKAN_CHECK(vkBindBufferMemory(g_VulkanDriver->GetDevice(), m_BufferHandle, m_BufferMemory, 0));
}

void VulkanBuffer::Release()
{
    // Destroy the buffer.
    vkDestroyBuffer(g_VulkanDriver->GetDevice(), m_BufferHandle, nullptr);
    m_BufferHandle = VK_NULL_HANDLE;

    // Destroy the memory allocated for the buffer storage.
    vkFreeMemory(g_VulkanDriver->GetDevice(), m_BufferMemory, nullptr);
    m_BufferMemory = VK_NULL_HANDLE;

    // Set the size to zero.
    m_BufferSize = 0;
}

void* VulkanBuffer::Map(usize offset, usize size)
{
    void* mappedBufferData = nullptr;
    SE_VULKAN_CHECK(vkMapMemory(g_VulkanDriver->GetDevice(), m_BufferMemory, offset, size, 0, &mappedBufferData));
    return mappedBufferData;
}

void VulkanBuffer::Unmap()
{
    vkUnmapMemory(g_VulkanDriver->GetDevice(), m_BufferMemory);
}

VulkanVertexBuffer::VulkanVertexBuffer(const VertexBufferInfo& info)
{
    const VkMemoryPropertyFlags memoryPropertyFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | // Allows the buffer memory block to be mapped to CPU space.
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT; // Automatic management of "availability and visibility on the host".

    // Create the buffer.
    m_Buffer.Invalidate(info.BufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryPropertyFlags);

    if (info.InitialDataSize > 0)
    {
        // Map the buffer data.
        void* mappedBufferData = m_Buffer.Map(0, info.InitialDataSize);

        // Copy the initial data to the mapped buffer data.
        MemoryCopy(mappedBufferData, info.InitialData, info.InitialDataSize);

        // Unmap the buffer data.
        m_Buffer.Unmap();
        mappedBufferData = nullptr;
    }
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
    : m_DataType(info.DataType)
    , m_IndexCount(info.IndexCount)
{
    // Calculate the buffer size based on the number of indices and their primitive underlaying type.
    const usize bufferSize = (usize)m_IndexCount * GetIndexDataTypeByteCount(m_DataType);

    // Set the memory propery flags.
    const VkMemoryPropertyFlags memoryPropertyFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | // Allows the buffer memory block to be mapped to CPU space.
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT; // Automatic management of "availability and visibility on the host".

    // Create the buffer.
    m_Buffer.Invalidate(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memoryPropertyFlags);

    if (info.InitialIndexCount > 0)
    {
        // Calculate the size of the inidial data buffer.
        const usize initialDataSize = (usize)info.InitialIndexCount * GetIndexDataTypeByteCount(m_DataType);

        // Map the buffer data.
        void* mappedBufferData = m_Buffer.Map(0, initialDataSize);

        // Copy the initial data to the mapped buffer data.
        MemoryCopy(mappedBufferData, info.InitialIndices, initialDataSize);

        // Unmap the buffer data.
        m_Buffer.Unmap();
        mappedBufferData = nullptr;
    }
}

}
