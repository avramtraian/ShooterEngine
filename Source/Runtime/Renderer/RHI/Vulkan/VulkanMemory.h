// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

uint32 FindMemoryTypeIndexForAllocation(uint32 memoryTypeBits, VkMemoryPropertyFlags flags);

}
