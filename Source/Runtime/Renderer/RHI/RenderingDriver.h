// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Renderer/RHI/RHICore.h>

#include <memory>

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
    static bool Initialize(const RenderingDriverInfo& info);
    static void Shutdown();

public:
    virtual std::unique_ptr<RenderingSurface> CreateSurface(const RenderingSurfaceInfo& info) = 0;
    
    virtual std::shared_ptr<CommandList>  CreateCommandList  (const CommandListInfo& info)  = 0;
    virtual std::shared_ptr<IndexBuffer>  CreateIndexBuffer  (const IndexBufferInfo& info)  = 0;
    virtual std::shared_ptr<Shader>       CreateShader       (const ShaderInfo& info)       = 0;
    virtual std::shared_ptr<Texture2D>    CreateTexture2D    (const Texture2DInfo& info)    = 0;
    virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer (const VertexBufferInfo& info) = 0;

private:
    virtual bool InitializeBackend(const RenderingDriverInfo& info) = 0;
    virtual void ShutdownBackend() = 0;
};

SHOOTER_API extern RenderingDriver* g_RenderingDriver;

}
