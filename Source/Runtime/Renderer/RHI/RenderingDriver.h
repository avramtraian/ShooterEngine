// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Renderer/RHI/Buffer.h>
#include <Runtime/Renderer/RHI/CommandList.h>
#include <Runtime/Renderer/RHI/RenderPass.h>
#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Shader.h>
#include <Runtime/Renderer/RHI/Synchronization.h>
#include <Runtime/Renderer/RHI/Texture.h>

namespace SE
{

enum class RenderingDriverBackend : uint8
{
    None = 0,
    Vulkan,
    MaxEnumValue,
};

struct RenderingDriverInfo
{
public:
    RenderingDriverBackend Backend  { RenderingDriverBackend::None };
    uint32 MaxCommandBuffersPerPool { 8 };

public:
    inline RenderingDriverInfo& SetBackend(RenderingDriverBackend backend) { Backend = backend; return *this; }
};

class RenderingDriver
{
    SE_MAKE_SINGLETON_CLASS(RenderingDriver);

public:
    RUNTIME_API static bool Initialize(const RenderingDriverInfo& info);
    RUNTIME_API static void Shutdown();

public:
    virtual RefPtr<RenderingSurface>            CreateSurface                       (const RenderingSurfaceInfo& info)          = 0;
    
    virtual RefPtr<CommandList>                 CreateCommandList                   (const CommandListInfo& info)               = 0;
    virtual RefPtr<IndexBuffer>                 CreateIndexBuffer                   (const IndexBufferInfo& info)               = 0;
    virtual RefPtr<RenderPass>                  CreateRenderPass                    (const RenderPassInfo& info)                = 0;
    virtual RefPtr<Shader>                      CreateShader                        (const ShaderInfo& info)                    = 0;
    virtual RefPtr<VertexBuffer>                CreateVertexBuffer                  (const VertexBufferInfo& info)              = 0;

    virtual RefPtr<Texture2D>                   CreateTexture2D                     (const Texture2DInfo& info)                 = 0;
    virtual RefPtr<UniformBuffer>               CreateUniformBuffer                 (const UniformBufferInfo& info)             = 0;

    virtual FenceHandle                         AcquireFence                        ()                                          = 0;
    virtual void                                RetireFence                         (FenceHandle fenceHandle)                   = 0;
    virtual SemaphoreHandle                     AcquireSemaphore                    ()                                          = 0;
    virtual void                                RetireSemaphore                     (SemaphoreHandle semaphoreHandle)           = 0;

public:
    virtual void                                ExecuteCommandList                  (const RefPtr<CommandList>& commandList,
                                                                                    const CommandListExecuteInfo& executeInfo)  = 0;
    virtual void                                ExecuteCommandListAndWait           (const RefPtr<CommandList>& commandList,
                                                                                    const CommandListExecuteInfo& executeInfo)  = 0;

    virtual void                                WaitForFence                        (FenceHandle fence, uint64 timeout)         = 0;
    virtual bool                                IsFenceSignaled                     (FenceHandle fence)                         = 0;
    virtual void                                ResetFence                          (FenceHandle fence)                         = 0;
    virtual void                                WaitForDeviceIdle                   ()                                          = 0;

private:
    virtual bool InitializeBackend(const RenderingDriverInfo& info) = 0;
    virtual void ShutdownBackend() = 0;
};

RUNTIME_API extern RenderingDriver* g_RenderingDriver;

}
