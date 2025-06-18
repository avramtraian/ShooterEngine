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
    WindowsWindowMode mode { WindowsWindowMode::Windowed };
    Optional<uint32> size_x;
    Optional<uint32> size_y;
};

class WindowsWindow
{
    SE_MAKE_NONCOPYABLE(WindowsWindow);
    SE_MAKE_NONMOVABLE(WindowsWindow);

public:
    SHOOTER_API WindowsWindow();
    SHOOTER_API ~WindowsWindow();

    SHOOTER_API bool initialize(const WindowsWindowInfo& info);
    SHOOTER_API void pump_messages();

public:
    /* Returns zero (false) if the window hasn't been initialized yet. */
    NODISCARD SHOOTER_API bool should_close() const;
    
    /* These return zero (0) if the window hasn't been initialized yet. */
    NODISCARD SHOOTER_API uint32 get_size_x() const;
    NODISCARD SHOOTER_API uint32 get_size_y() const;

    /* Returns zero (nullptr) if the window hasn't been initialized yet. */
    NODISCARD SHOOTER_API void* get_native_handle() const;

private:
    static void win32_register_window_class();
    static LRESULT win32_window_procedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

    /* Returns a null pointer if no window with the specified native handle is found in the internal table. */
    static WindowsWindow* find_window_from_handle(HWND native_handle);

private:
    static Vector<WindowsWindow*> s_window_table;

private:
    HWND m_native_handle;
    bool m_should_close;
};

}
