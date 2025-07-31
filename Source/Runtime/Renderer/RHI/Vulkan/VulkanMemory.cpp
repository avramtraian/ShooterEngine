// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanMemory.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

uint32 FindMemoryTypeIndexForAllocation(uint32 memoryTypeBits, VkMemoryPropertyFlags flags)
{
    const VkPhysicalDeviceMemoryProperties& memoryProperties = g_VulkanDriver->GetPhysicalDevice().MemoryProperties;
    for (uint32 memoryTypeIndex = 0; memoryTypeIndex < memoryProperties.memoryTypeCount; ++memoryTypeIndex)
    {
        // First, check if the index is compatible with the required memory type index.
        // Second ,check if the memory type index has the required flags.
        if ((memoryTypeBits & (1 << memoryTypeIndex)) &&
            ((memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & flags) == flags))
        {
            return memoryTypeIndex;
        }
    }

    SE_ASSERT(!"No memory type was found for the specified configuration!");
    return (uint32)(-1);
}

}
