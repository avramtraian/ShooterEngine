// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/GraphicsState.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

#include <memory>

namespace SE
{

class VulkanPipeline : public RefCounted
{
public:
    VulkanPipeline(const GraphicsState& graphicsState, VkRenderPass renderPassHandle, uint32 colorAttachmentCount);
    virtual ~VulkanPipeline() override;

public:
    NODISCARD FORCEINLINE VkPipeline GetHandle() const { return m_Handle; }
    NODISCARD FORCEINLINE const GraphicsState& GetGraphicsState() const { return m_GraphicsState; }

    NODISCARD bool IsCompatibleWithGraphicsState(const GraphicsState& graphicsState) const;

private:
    void InvalidatePipeline(VkRenderPass renderPassHandle, uint32 colorAttachmentCount);

private:
    VkPipeline m_Handle;
    GraphicsState m_GraphicsState;
};

}
