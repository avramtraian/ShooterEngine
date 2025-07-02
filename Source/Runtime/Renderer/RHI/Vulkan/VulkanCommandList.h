// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/CommandList.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

class VulkanCommandList : public CommandList
{
public:
    VulkanCommandList(const CommandListInfo& info);
    virtual ~VulkanCommandList() override;

    virtual void BeginRenderPass(const RenderPassInfo& renderPassInfo) override;
    virtual void EndRenderPass() override;

private:

private:
    VkCommandBuffer m_CommandBuffer;
    std::shared_ptr<VulkanCommandPool> m_ParentCommandPool;
    
    VkRenderPass m_ActiveRenderPass;
    VkFramebuffer m_ActiveFramebuffer;
};

}
