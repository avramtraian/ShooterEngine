// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RenderPass.h>
#include <Runtime/Renderer/RHI/Buffer.h>
#include <Runtime/Renderer/RHI/Synchronization.h>
#include <Runtime/Renderer/RHI/Texture.h>

namespace SE
{

enum PipelineStageBitsEnum : uint64
{
    PIPELINE_STAGE_NONE_BIT = 0,
    PIPELINE_STAGE_TOP_OF_PIPE_BIT             = BIT(0),
    PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = BIT(1),
};
using PipelineStageBits = uint64;

struct CommandListExecuteInfo
{
public:
    std::vector<SemaphoreHandle> WaitSemaphores;
    std::vector<PipelineStageBits> WaitStageBits;
    std::vector<SemaphoreHandle> SignalSemaphores;
    FenceHandle SignalFence { nullptr };

public:
    inline CommandListExecuteInfo& AddSignalSemaphore (SemaphoreHandle semaphore) { SignalSemaphores.push_back(semaphore); return *this; }
    inline CommandListExecuteInfo& SetSignalFence     (FenceHandle fence)         { SignalFence = fence;                   return *this; }
    
    inline CommandListExecuteInfo& AddWaitSemaphore   (SemaphoreHandle semaphore, PipelineStageBits stageBits)
    {
        WaitSemaphores.push_back(semaphore);
        WaitStageBits.push_back(stageBits);
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

    virtual void BindGraphicsState(const GraphicsState& graphicsState) = 0;

    virtual void BindVertexBuffer(const RefPtr<VertexBuffer>& vertexBuffer) = 0;
    virtual void BindIndexBuffer(const RefPtr<IndexBuffer>& indexBuffer) = 0;

    virtual void DrawIndexed(uint32 firstIndex, uint32 indexCount) = 0;

public:
    NODISCARD virtual const DrawStatistics& GetDrawStatistics() const = 0;
    virtual void ResetDrawStatistics() = 0;

    virtual void SetAccumulateStatisticsPolicy(AccumultateStatisticsPolicy policy) = 0;
    NODISCARD virtual AccumultateStatisticsPolicy GetAccumulateStatisticsPolicy() const = 0;
};

}
