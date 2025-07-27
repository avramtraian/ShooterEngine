// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Synchronization.h>

#include <memory>

namespace SE
{

/* Forward declarations. */
class Window;

struct RenderingSurfaceInfo
{
public:
    Window* OwningWindow;
    uint32 SwapchainImageCount;
    bool EnableVSync;

public:
    inline RenderingSurfaceInfo& SetOwningWindow        (Window* window)    { OwningWindow = window;            return *this; }
    inline RenderingSurfaceInfo& SetSwapchainImageCount (uint32 imageCount) { SwapchainImageCount = imageCount; return *this; }
    inline RenderingSurfaceInfo& SetEnableVSync         (bool enableVSync)  { EnableVSync = enableVSync;        return *this; }
};

class RenderingSurface
{
    SE_MAKE_RENDERER_RHI_INTERFACE(RenderingSurface);

public:
    virtual bool Invalidate() = 0;

    NODISCARD virtual uint32 GetMaxFramesInFlight() const = 0;
    NODISCARD virtual uint32 GetCurrentFrameIndex() const = 0;
    NODISCARD virtual uint32 GetCurrentSwapchainImageIndex() const = 0;

    NODISCARD virtual uint32 GetSurfaceSizeX() const = 0;
    NODISCARD virtual uint32 GetSurfaceSizeY() const = 0;

    virtual std::shared_ptr<Texture2D> GetSurfaceTexture2D(uint32 imageIndex) = 0;
    virtual std::shared_ptr<Texture2D> GetCurrentSurfaceTexture2D() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual SemaphoreHandle GetImageAvailableSemaphore() = 0;
    virtual SemaphoreHandle GetRenderFinishedSemaphore() = 0;
    virtual FenceHandle GetRenderFinishedFence() = 0;
};

}
