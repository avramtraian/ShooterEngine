// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Synchronization.h>
#include <Runtime/Renderer/RHI/Texture.h>

namespace SE
{

struct RenderingSurfaceInfo
{
public:
    RefPtr<Window> OwningWindow           { nullptr };
    uint32         SwapchainMinImageCount { 2 };
    uint32         MaxFramesInFlight      { 1 };

public:
    inline RenderingSurfaceInfo& SetOwningWindow           (RefPtr<Window> window) { OwningWindow = Move(window);         return *this; }
    inline RenderingSurfaceInfo& SetSwapchainMinImageCount (uint32 imageCount)     { SwapchainMinImageCount = imageCount; return *this; }
    inline RenderingSurfaceInfo& SetMaxFramesInFlight      (uint32 frameCount)     { MaxFramesInFlight = frameCount;      return *this; }
};

class RenderingSurface
{
    SE_MAKE_NONCOPYABLE(RenderingSurface);
    SE_MAKE_NONMOVABLE(RenderingSurface);

public:
    RenderingSurface() = default;
    virtual ~RenderingSurface() = default;

public:
    virtual bool Invalidate() = 0;

    NODISCARD virtual uint32 GetMaxFramesInFlight() const = 0;
    NODISCARD virtual uint32 GetCurrentFrameIndex() const = 0;
    NODISCARD virtual uint32 GetCurrentSwapchainImageIndex() const = 0;

    NODISCARD virtual TextureFormat GetSurfaceFormat() const = 0;
    NODISCARD virtual uint32 GetSurfaceSizeX() const = 0;
    NODISCARD virtual uint32 GetSurfaceSizeY() const = 0;

    virtual RefPtr<Texture2D> GetSurfaceTexture2D(uint32 imageIndex) = 0;
    virtual RefPtr<Texture2D> GetCurrentSurfaceTexture2D() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame(bool waitForRenderFinishedSemaphore) = 0;

    virtual SemaphoreHandle GetImageAvailableSemaphore() = 0;
    virtual SemaphoreHandle GetRenderFinishedSemaphore() = 0;
    virtual FenceHandle GetRenderFinishedFence() = 0;
};

}
