// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/Core/Containers/String.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Platform/PlatformCoreInclude.h>

namespace SE
{

enum class WindowsWindowMode : uint8
{
    Windowed,
    Maximized,
    Minimized,
};

struct WindowsWindowInfo
{
public:
    WindowsWindowMode StartMode { WindowsWindowMode::Windowed };
    bool Fullscreen { false };
    String Title { VIEW("Unnamed Window") };
    Optional<int32> PositionX;
    Optional<int32> PositionY;
    Optional<uint32> SizeX;
    Optional<uint32> SizeY;

public:
    inline WindowsWindowInfo& SetStartMode  (WindowsWindowMode value)    { StartMode = value;                  return *this; }
    inline WindowsWindowInfo& SetFullscreen (bool fullscreen)            { Fullscreen = fullscreen;            return *this; }
    inline WindowsWindowInfo& SetTitle      (StringView title)           { Title = title;                      return *this; }
    inline WindowsWindowInfo& SetPosition   (uint32 posX, uint32 posY)   { PositionX = posX; PositionY = posY; return *this; }
    inline WindowsWindowInfo& SetSize       (uint32 sizeX, uint32 sizeY) { SizeX = sizeX; SizeY = sizeY;       return *this; }
};

class WindowsWindow
{
    SE_MAKE_NONCOPYABLE(WindowsWindow);
    SE_MAKE_NONMOVABLE(WindowsWindow);

public:
    SHOOTER_API WindowsWindow();
    SHOOTER_API ~WindowsWindow();

    SHOOTER_API bool Initialize(const WindowsWindowInfo& info);
    SHOOTER_API void PumpMessages();

public:
    /* Returns zero (false) if the window hasn't been initialized yet. */
    NODISCARD SHOOTER_API bool ShouldClose() const;
    
    /* These return zero (0) if the window hasn't been initialized yet. */
    NODISCARD SHOOTER_API uint32 GetSizeX() const;
    NODISCARD SHOOTER_API uint32 GetSizeY() const;

    /* Returns zero (nullptr) if the window hasn't been initialized yet. */
    NODISCARD SHOOTER_API void* GetNativeHandle() const;

private:
    static void Win32RegisterWindowClass();
    static LRESULT Win32WindowProcedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

    /* Returns a null pointer if no window with the specified native handle is found in the internal table. */
    static WindowsWindow* FindWindowFromHandle(HWND native_handle);

private:
    static Vector<WindowsWindow*> s_WindowTable;

private:
    HWND m_NativeHandle;
    bool m_ShouldClose;
};

}
