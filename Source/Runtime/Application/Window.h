// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/Core/Containers/RefPtr.h>
#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Delegate.h>

namespace SE
{

enum class WindowMode : uint8
{
    Windowed,
    Fullscreen,
    Minimized,
    Maximized,
};

struct WindowInfo
{
public:
    WindowMode       StartMode { WindowMode::Windowed };
    String           Title     { VIEW("Unnamed Shooter Window") };
    Optional<uint32> SizeX;
    Optional<uint32> SizeY;
    Optional<int32>  PositionX;
    Optional<int32>  PositionY;

public:
    inline WindowInfo& SetStartMode (WindowMode value)   { StartMode = value;            return *this; }
    inline WindowInfo& SetTitle     (StringView value)   { Title = value;                return *this; }
    inline WindowInfo& SetSize      (uint32 x, uint32 y) { SizeX = x; SizeY = y;         return *this; }
    inline WindowInfo& SetPosition  (int32 x, int32 y)   { PositionX = x; PositionY = y; return *this; }
};

//
// @param (RefPtr<Window>) : The window that was resized.
// @param (uint32)         : The new size on the X-axis (width) of the window.
// @param (uint32)         : The new size on the Y-axis (height) of the window.
//
SE_DECLARE_MULTICAST_DELEGATE_THREE_PARAMS(OnWindowResizedDelegate, RefPtr<class Window>, uint32, uint32);

//
// @param (RefPtr<Window>) : The window that was closed.
//
SE_DECLARE_MULTICAST_DELEGATE_ONE_PARAM(OnWindowClosedDelegate, RefPtr<class Window>);

//
// @param (RefPtr<Window>) : The window from where the event was received.
// @param (float)          : The scroll offset.
//
SE_DECLARE_MULTICAST_DELEGATE_TWO_PARAMS(OnMouseWheelScrolledDelegate, RefPtr<class Window>, float);

class Window : public RefCounted
{
public:
    NODISCARD SHOOTER_API static RefPtr<Window> Create(const WindowInfo& info);

public:
    Window() = default;
    virtual ~Window() = default;

public:
    NODISCARD virtual uint32 GetSizeX() const = 0;
    NODISCARD virtual uint32 GetSizeY() const = 0;
    NODISCARD virtual WindowMode GetCurrentMode() const = 0;
    
    NODISCARD virtual void* GetNativeHandle() const = 0;
    NODISCARD virtual bool IsRequestedToClose() const = 0;

    NODISCARD virtual OnWindowResizedDelegate& GetOnWindowResizedDelegate() = 0;
    NODISCARD virtual OnWindowClosedDelegate& GetOnWindowClosedDelegate() = 0;

    NODISCARD virtual OnMouseWheelScrolledDelegate& GetOnMouseWheelScrolledDelegate() = 0;

public:
    virtual void SetSize(Optional<uint32> sizeX, Optional<uint32> sizeY) = 0;
    virtual void SetCurrentMode(WindowMode mode) = 0;

    virtual void ProcessEventQueue() = 0;
    virtual void Close() = 0;
};

}
