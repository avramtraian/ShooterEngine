/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Runtime/Core/Containers/RefPtr.h>
#include <Runtime/Renderer/RHI/Texture.h>
#include <Runtime/Renderer/Renderer2D.h>
#include <Runtime/Scene/Scene.h>

namespace SE
{

struct SceneRendererInfo
{
public:
    StrongRefPtr<Scene> SceneContext;
    uint32              MaxFramesInFlight       = 0;
    bool                RenderToSwapchainTarget = false;
    TextureFormat       RenderTargetFormat      = TextureFormat::Unknown;

public:
    // clang-format off
    inline SceneRendererInfo& SetSceneContext            (StrongRefPtr<Scene> sceneContext) { SceneContext = Move(sceneContext);     return *this; }
    inline SceneRendererInfo& SetMaxFramesInFlight       (uint32 maxFramesInFlight)         { MaxFramesInFlight = maxFramesInFlight; return *this; }
    inline SceneRendererInfo& SetRenderToSwapchainTarget (bool value)                       { RenderToSwapchainTarget = value;       return *this; }
    inline SceneRendererInfo& SetRenderTargetFormat      (TextureFormat format)             { RenderTargetFormat = format;           return *this; }
    // clang-format on
};

struct SceneRendererRenderInfo
{
public:
    StrongRefPtr<Texture2D>   RenderTarget;
    StrongRefPtr<CommandList> CommandList;
    SemaphoreHandle           ImageAvailableSemaphore {};
    SemaphoreHandle           RenderFinishedSemaphore {};
    FenceHandle               RenderFinishedFence {};
    uint32                    FrameIndex {};

public:
    // clang-format off
    inline SceneRendererRenderInfo& SetRenderTarget            (RefPtr<Texture2D> renderTarget)        { RenderTarget = Move(renderTarget);   return *this; }
    inline SceneRendererRenderInfo& SetCommandList             (RefPtr<class CommandList> commandList) { CommandList = Move(commandList);     return *this; }
    inline SceneRendererRenderInfo& SetImageAvailableSemaphore (SemaphoreHandle semaphore)             { ImageAvailableSemaphore = semaphore; return *this; }
    inline SceneRendererRenderInfo& SetRenderFinishedSemaphore (SemaphoreHandle semaphore)             { RenderFinishedSemaphore = semaphore; return *this; }
    inline SceneRendererRenderInfo& SetRenderFinishedFence     (FenceHandle fence)                     { RenderFinishedFence = fence;         return *this; }
    inline SceneRendererRenderInfo& SetFrameIndex              (uint32 frameIndex)                     { FrameIndex = frameIndex;             return *this; }
    // clang-format on
};

class SceneRenderer : public RefCounted
{
public:
    NODISCARD RUNTIME_API static RefPtr<SceneRenderer> Create(const SceneRendererInfo& info);

public:
    virtual ~SceneRenderer() override = default;

    RUNTIME_API void Render(SceneRendererRenderInfo renderInfo);

private:
    SceneRenderer(const SceneRendererInfo& info);

private:
    StrongRefPtr<Scene>      m_SceneContext;
    StrongRefPtr<Renderer2D> m_Renderer2D;
};

} // namespace SE
