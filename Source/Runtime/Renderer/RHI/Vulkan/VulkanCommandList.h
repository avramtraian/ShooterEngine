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

    NODISCARD FORCEINLINE virtual CommandListFamily GetFamily() const override { return m_Family; }
    NODISCARD FORCEINLINE VkCommandBuffer GetHandle() const { return m_CommandBuffer; }

public:
    virtual void Begin() override;
    virtual void End() override;

    virtual void BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass, const RenderPassBeginInfo& beginInfo) override;
    virtual void EndRenderPass() override;

    virtual void BindGraphicsState(const GraphicsState& graphicsState) override;

    virtual void BindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
    virtual void BindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    virtual void DrawIndexed(uint32 firstIndex, uint32 indexCount) override;

public:
    NODISCARD FORCEINLINE virtual const DrawStatistics& GetDrawStatistics() const override { return m_DrawStatistics; }
    NODISCARD FORCEINLINE virtual AccumultateStatisticsPolicy GetAccumulateStatisticsPolicy() const override { return m_AccumulateStatisticsPolicy; }

    virtual void ResetDrawStatistics() override;
    virtual void SetAccumulateStatisticsPolicy(AccumultateStatisticsPolicy policy) override;

private:
    NODISCARD bool ValidateRenderPass(const std::shared_ptr<VulkanRenderPass>& renderPass, const RenderPassBeginInfo& beginInfo) const;

private:
    VkCommandBuffer m_CommandBuffer;
    std::shared_ptr<VulkanCommandPool> m_ParentCommandPool;
    CommandListFamily m_Family;

    std::shared_ptr<VulkanRenderPass> m_ActiveRenderPass;
    std::vector<std::shared_ptr<VulkanRenderPass>> m_UsedRenderPasses;
    VulkanFramebuffer* m_ActiveFramebuffer;
    VulkanPipeline* m_ActivePipeline;

    std::vector<std::shared_ptr<VulkanVertexBuffer>> m_UsedVertexBuffers;
    std::vector<std::shared_ptr<VulkanIndexBuffer>> m_UsedIndexBuffers;
    bool m_IsVertexBufferBound;
    bool m_IsIndexBufferBound;

    DrawStatistics m_DrawStatistics;
    AccumultateStatisticsPolicy m_AccumulateStatisticsPolicy;
};

}
