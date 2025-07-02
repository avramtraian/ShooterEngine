// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer./RHI/Vulkan/VulkanPipeline.h>
#include <Runtime/Renderer./RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

VulkanPipeline::VulkanPipeline(const PipelineInfo& info)
    : m_Handle(VK_NULL_HANDLE)
{}

VulkanPipeline::~VulkanPipeline()
{}

}
