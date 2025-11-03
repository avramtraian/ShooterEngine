// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/LockPtr.h>
#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Renderer/RHI/CommandList.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSet.h>

namespace SE
{

class VulkanCommandList : public CommandList
{
public:
    VulkanCommandList(const CommandListInfo& info);
    virtual ~VulkanCommandList() override;

    NODISCARD ALWAYS_INLINE virtual CommandListFamily GetFamily() const override { return m_Family; }
    NODISCARD ALWAYS_INLINE VkCommandBuffer GetHandle() const { return m_CommandBuffer; }

public:
    virtual void Begin() override;
    virtual void End() override;

    virtual void BeginRenderPass(const RefPtr<RenderPass>& renderPass, const RenderPassBeginInfo& beginInfo) override;
    virtual void EndRenderPass() override;

    virtual void BindGraphicsState(const GraphicsState& graphicsState, const RefPtr<Shader>& shader) override;

    virtual void BindShaderResources(const ShaderResourcesBindPack& bindPack) override;

    virtual void BindVertexBuffer(const RefPtr<VertexBuffer>& vertexBuffer) override;
    virtual void BindIndexBuffer(const RefPtr<IndexBuffer>& indexBuffer) override;

    virtual void DrawIndexed(uint32 firstIndex, uint32 indexCount) override;

public:
    virtual void TransitionTexture(const TransitionTextureInfo& info) override;

    void CopyBufferToImage(const RefPtr<VulkanTexture2D>& dstTexture, VkBuffer srcBuffer);

public:
    NODISCARD ALWAYS_INLINE virtual const DrawStatistics& GetDrawStatistics() const override { return m_DrawStatistics; }
    NODISCARD ALWAYS_INLINE virtual AccumultateStatisticsPolicy GetAccumulateStatisticsPolicy() const override { return m_AccumulateStatisticsPolicy; }

    virtual void ResetDrawStatistics() override;
    virtual void SetAccumulateStatisticsPolicy(AccumultateStatisticsPolicy policy) override;

private:
    void ReleaseObjectReferences();

    NODISCARD bool ValidateRenderPass(const RefPtr<VulkanRenderPass>& renderPass, const RenderPassBeginInfo& beginInfo) const;
    NODISCARD bool ValidateShaderResourcesBindPack(const RefPtr<VulkanShader>& shader, const ShaderResourcesBindPack& bindPack) const;
    
    NODISCARD VulkanBuffer& CreateStagingBuffer(usize bufferSize);

private:
    VkCommandBuffer m_CommandBuffer;
    RefPtr<VulkanCommandPool> m_ParentCommandPool;
    CommandListFamily m_Family;

    RefPtr<VulkanRenderPass> m_ActiveRenderPass;
    LockPtr<VulkanFramebuffer> m_ActiveFramebuffer;
    LockPtr<VulkanPipeline> m_ActivePipeline;

    Vector<RefPtr<VulkanRenderPass>> m_UsedRenderPasses;
    Vector<RefPtr<VulkanVertexBuffer>> m_UsedVertexBuffers;
    Vector<RefPtr<VulkanIndexBuffer>> m_UsedIndexBuffers;

    Vector<LockPtr<VulkanFramebuffer>> m_UsedFramebuffers;
    Vector<LockPtr<VulkanPipeline>> m_UsedPipelines;
    Vector<LockPtr<VulkanDescriptorSet>> m_UsedDescriptorSets;

    bool m_IsVertexBufferBound;
    bool m_IsIndexBufferBound;

    Vector<RefPtr<VulkanTexture2D>> m_TransitionedTextures;
    Vector<OwnPtr<VulkanBuffer>> m_StagingBuffers;

    DrawStatistics m_DrawStatistics;
    AccumultateStatisticsPolicy m_AccumulateStatisticsPolicy;
};

}
