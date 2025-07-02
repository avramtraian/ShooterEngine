// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/Pipeline.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

class VulkanPipeline : public Pipeline
{
public:
    VulkanPipeline(const PipelineInfo& info);
    virtual ~VulkanPipeline() override;

private:
    VkPipeline m_Handle;
};

}
