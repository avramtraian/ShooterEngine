// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RenderPass.h>
#include <Runtime/Renderer/RHI/Buffer.h>
#include <Runtime/Renderer/RHI/Synchronization.h>
#include <Runtime/Renderer/RHI/Shader.h>
#include <Runtime/Renderer/RHI/Texture.h>

namespace SE
{

struct CommandListExecuteInfo
{
public:
    Vector<SemaphoreHandle> WaitSemaphores;
    Vector<PipelineStageBits> WaitStageBits;
    Vector<SemaphoreHandle> SignalSemaphores;
    FenceHandle SignalFence { nullptr };

public:
    inline CommandListExecuteInfo& AddSignalSemaphore (SemaphoreHandle semaphore) { SignalSemaphores.Add(semaphore); return *this; }
    inline CommandListExecuteInfo& SetSignalFence     (FenceHandle fence)         { SignalFence = fence;             return *this; }
    
    inline CommandListExecuteInfo& AddWaitSemaphore   (SemaphoreHandle semaphore, PipelineStageBits stageBits)
    {
        WaitSemaphores.Add(semaphore);
        WaitStageBits.Add(stageBits);
        return *this;
    }
};

enum class CommandListFamily : uint8
{
    Unknown = 0,
    Graphics,
    Transfer,
    Compute,
};

struct TransitionTextureInfo
{
public:
    RefPtr<Texture2D> Texture;
    TextureLayout     OldLayout         { TextureLayout::Undefined };
    TextureLayout     NewLayout         { TextureLayout::Undefined };
    PipelineStageBits SrcPipelineStages { PIPELINE_STAGE_TOP_OF_PIPE_BIT };
    PipelineStageBits DstPipelineStages { PIPELINE_STAGE_TOP_OF_PIPE_BIT };
    AccessFlagsBits   SrcAccessFlags    { ACCESS_FLAG_NONE_BIT };
    AccessFlagsBits   DstAccessFlags    { ACCESS_FLAG_NONE_BIT };

public:
    inline TransitionTextureInfo& SetTexture           (const RefPtr<Texture2D>& texture) { Texture = texture;             return *this; }
    inline TransitionTextureInfo& SetOldLayout         (TextureLayout layout)             { OldLayout = layout;            return *this; }
    inline TransitionTextureInfo& SetNewLayout         (TextureLayout layout)             { NewLayout = layout;            return *this; }
    inline TransitionTextureInfo& SetSrcPipelineStages (PipelineStageBits stages)         { SrcPipelineStages = stages;    return *this; }
    inline TransitionTextureInfo& SetDstPipelineStages (PipelineStageBits stages)         { DstPipelineStages = stages;    return *this; }
    inline TransitionTextureInfo& AddSrcPipelineStages (PipelineStageBits stages)         { SrcPipelineStages |= stages;   return *this; }
    inline TransitionTextureInfo& AddDstPipelineStages (PipelineStageBits stages)         { DstPipelineStages |= stages;   return *this; }
    inline TransitionTextureInfo& SetSrcAccessFlags    (AccessFlagsBits accessFlags)      { SrcAccessFlags = accessFlags;  return *this; }
    inline TransitionTextureInfo& SetDstAccessFlags    (AccessFlagsBits accessFlags)      { DstAccessFlags = accessFlags;  return *this; }
    inline TransitionTextureInfo& AddSrcAccessFlags    (AccessFlagsBits accessFlags)      { SrcAccessFlags |= accessFlags; return *this; }
    inline TransitionTextureInfo& AddDstAccessFlags    (AccessFlagsBits accessFlags)      { DstAccessFlags |= accessFlags; return *this; }

    inline TransitionTextureInfo& SetLayouts           (TextureLayout oldLayout, TextureLayout newLayout)
    {
        OldLayout = oldLayout;
        NewLayout = newLayout;
        return *this;
    }

    inline TransitionTextureInfo& SetPipelineStages    (PipelineStageBits srcStages, PipelineStageBits dstStages)
    {
        SrcPipelineStages = srcStages;
        DstPipelineStages = dstStages;
        return *this;
    }

    inline TransitionTextureInfo& SetAccessFlags       (AccessFlagsBits srcAccessFlags, AccessFlagsBits dstAccessFlags)
    {
        SrcAccessFlags = srcAccessFlags;
        DstAccessFlags = dstAccessFlags;
        return *this;
    }
};

struct ShaderResourceTexture
{
public:
    uint32 SetIndex { 0 };
    uint32 BindingIndex { 0 };
    RefPtr<Texture2D> Texture;

public:
    inline ShaderResourceTexture& SetSetIndex     (uint32 setIndex)           { SetIndex = setIndex;          return *this; }
    inline ShaderResourceTexture& SetBindingIndex (uint32 bindingIndex)       { BindingIndex = bindingIndex;  return *this; }
    inline ShaderResourceTexture& SetTexture      (RefPtr<Texture2D> texture) { Texture = std::move(texture); return *this; }
};

struct ShaderResourceUniformBuffer
{
public:
    uint32 SetIndex { 0 };
    uint32 BindingIndex { 0 };
    RefPtr<UniformBuffer> Buffer;

public:
    inline ShaderResourceUniformBuffer& SetSetIndex      (uint32 setIndex)       { SetIndex = setIndex;         return *this; }
    inline ShaderResourceUniformBuffer& SetBindingIndex  (uint32 bindingIndex)   { BindingIndex = bindingIndex; return *this; }
    inline ShaderResourceUniformBuffer& SetBuffer (RefPtr<UniformBuffer> buffer) { Buffer = std::move(buffer);  return *this; }
};

struct ShaderResourcesBindPack
{
public:
    Vector<ShaderResourceTexture> Textures;
    Vector<ShaderResourceUniformBuffer> UniformBuffers;

public:
    inline ShaderResourcesBindPack& AddTexture       (ShaderResourceTexture texture)             { Textures.Add(std::move(texture));             return *this; }
    inline ShaderResourcesBindPack& AddUniformBuffer (ShaderResourceUniformBuffer uniformBuffer) { UniformBuffers.Add(std::move(uniformBuffer)); return *this; }
};

struct CommandListInfo
{
public:
    CommandListFamily Family { CommandListFamily::Unknown };

public:
    inline CommandListInfo& SetFamily(CommandListFamily family) { Family = family; return *this; }
};

class CommandList : public RefCounted
{
    SE_MAKE_RENDERER_RHI_INTERFACE(CommandList);

public:
    struct DrawStatistics
    {
        uint32 DrawCalls { 0 };
        uint32 Triangles { 0 };
        uint32 Vertices { 0 };
    };

    enum class AccumultateStatisticsPolicy : uint8
    {
        PerBeginEndCycle,
        PerLifetime,
    };

public:
    NODISCARD virtual CommandListFamily GetFamily() const = 0;

    virtual void Begin() = 0;
    virtual void End() = 0;

    virtual void BeginRenderPass(const RefPtr<RenderPass>& renderPass, const RenderPassBeginInfo& beginInfo) = 0;
    virtual void EndRenderPass() = 0;

    virtual void BindGraphicsState(const GraphicsState& graphicsState, const RefPtr<Shader>& shader) = 0;

    virtual void BindShaderResources(const ShaderResourcesBindPack& bindInfo) = 0;

    virtual void BindVertexBuffer(const RefPtr<VertexBuffer>& vertexBuffer) = 0;
    virtual void BindIndexBuffer(const RefPtr<IndexBuffer>& indexBuffer) = 0;

    virtual void DrawIndexed(uint32 firstIndex, uint32 indexCount) = 0;

public:
    virtual void TransitionTexture(const TransitionTextureInfo& info) = 0;

public:
    NODISCARD virtual const DrawStatistics& GetDrawStatistics() const = 0;
    virtual void ResetDrawStatistics() = 0;

    virtual void SetAccumulateStatisticsPolicy(AccumultateStatisticsPolicy policy) = 0;
    NODISCARD virtual AccumultateStatisticsPolicy GetAccumulateStatisticsPolicy() const = 0;
};

}
