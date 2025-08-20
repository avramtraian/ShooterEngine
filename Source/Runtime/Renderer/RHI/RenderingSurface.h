// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Synchronization.h>
#include <Runtime/Renderer/RHI/Texture.h>

#include <memory>

namespace SE
{

/* Forward declarations. */
class Window;

struct RenderingSurfaceInfo
{
public:
    Window* OwningWindow           { nullptr };
    uint32  SwapchainMinImageCount { 2 };
    uint32  MaxFramesInFlight      { 1 };
    bool    EnableVSync            { false };

public:
    inline RenderingSurfaceInfo& SetOwningWindow           (Window* window)    { OwningWindow = window;               return *this; }
    inline RenderingSurfaceInfo& SetSwapchainMinImageCount (uint32 imageCount) { SwapchainMinImageCount = imageCount; return *this; }
    inline RenderingSurfaceInfo& SetMaxFramesInFlight      (uint32 frameCount) { MaxFramesInFlight = frameCount;      return *this; }
    inline RenderingSurfaceInfo& SetEnableVSync            (bool enableVSync)  { EnableVSync = enableVSync;           return *this; }
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
    virtual void EndFrame() = 0;

    virtual SemaphoreHandle GetImageAvailableSemaphore() = 0;
    virtual SemaphoreHandle GetRenderFinishedSemaphore() = 0;
    virtual FenceHandle GetRenderFinishedFence() = 0;
};

}
