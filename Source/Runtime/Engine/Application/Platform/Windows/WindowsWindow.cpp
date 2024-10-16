/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/Optional.h>
#include <Engine/Application/Platform/Windows/WindowsWindow.h>
#include <Engine/Application/WindowEvent.h>
#include <Engine/Engine.h>

namespace SE
{

Vector<WindowsWindow*> WindowsWindow::s_windows;

static const char* s_window_class_name = "ShooterWindowClass";
static bool s_window_class_was_registered = false;

static void register_window_class()
{
    if (s_window_class_was_registered)
        return;
    s_window_class_was_registered = true;

    WNDCLASSA window_class = {};
    window_class.hInstance = GetModuleHandle(nullptr);
    window_class.lpszClassName = s_window_class_name;
    window_class.lpfnWndProc = WindowsWindow::window_procedure;
    RegisterClassA(&window_class);
}

static DWORD get_window_style_flags(const WindowInfo& window_info)
{
    if (window_info.start_maximized && window_info.start_minimized)
        return 0;

    DWORD style_flags = WS_VISIBLE;

    if (window_info.start_maximized)
        style_flags |= WS_MAXIMIZE;
    if (window_info.start_minimized)
        style_flags |= WS_MINIMIZE;

    if (window_info.start_fullscreen)
        style_flags |= 0;
    else
        style_flags |= WS_OVERLAPPEDWINDOW;

    return style_flags;
}

bool WindowsWindow::initialize(const WindowInfo& window_info)
{
    if (m_native_handle != nullptr)
        return false;
    register_window_class();

    m_event_callback = window_info.event_callback;
    m_native_event_callback = window_info.native_event_callback;

    const char* window_title = window_info.title.byte_span_with_null_termination().as<const char>().elements();
    const DWORD window_style_flags = get_window_style_flags(window_info);
    if (window_style_flags == 0)
        return false;

    m_should_close = false;
    m_native_handle = CreateWindowA(
        s_window_class_name,
        window_title,
        window_style_flags,
        window_info.position_x,
        window_info.position_y,
        window_info.width,
        window_info.height,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (m_native_handle == nullptr)
        return false;

    return true;
}

WindowsWindow::WindowsWindow()
{
    s_windows.add(this);
}

WindowsWindow::~WindowsWindow()
{
    DestroyWindow(m_native_handle);

    Optional<usize> window_index;
    for (usize index = 0; index < s_windows.count(); ++index)
    {
        if (s_windows[index] == this)
        {
            window_index = index;
            break;
        }
    }

    SE_ASSERT(window_index.has_value());
    s_windows.remove_unordered(*window_index);
}

void WindowsWindow::pump_messages()
{
    MSG message = {};
    while (PeekMessageA(&message, m_native_handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

LRESULT WindowsWindow::window_procedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    WindowsWindow* window = get_window_from_native_handle(window_handle);
    if (window)
    {
        if (window->m_native_event_callback)
        {
            struct NativeEventData
            {
                HWND window_handle;
                UINT message;
                WPARAM w_param;
                LPARAM l_param;
            };

            NativeEventData native_event_data { window_handle, message, w_param, l_param };
            window->m_native_event_callback(&native_event_data);
        }

        switch (message)
        {
            case WM_QUIT:
            case WM_CLOSE:
            {
                window->m_should_close = true;
                return 0;
            }

            case WM_SIZE:
            {
                u32 new_width = LOWORD(l_param);
                u32 new_height = HIWORD(l_param);

                if (window->m_client_area_width != new_width || window->m_client_area_height != new_height)
                {
                    if (new_width == 0 || new_height == 0)
                    {
                        // The window has been minimized. We don't want to propagate window resized events,
                        // nor update the window dimensions.
                        return 0;
                    }

                    window->m_client_area_width = new_width;
                    window->m_client_area_height = new_height;

                    if (window->m_event_callback)
                    {
                        WindowResizedEvent window_event = WindowResizedEvent(new_width, new_height);
                        window->m_event_callback(window_event);
                    }
                }

                return 0;
            }

            case WM_MOVE:
            {
                POINTS new_position = MAKEPOINTS(l_param);

                if (window->m_client_area_position_x != new_position.x || window->m_client_area_position_y != new_position.y)
                {
                    window->m_client_area_position_x = new_position.x;
                    window->m_client_area_position_y = new_position.y;
                    // TODO: Propagate window position changed event.
                }

                return 0;
            }
        }
    }

    return DefWindowProcA(window_handle, message, w_param, l_param);
}

WindowsWindow* WindowsWindow::get_window_from_native_handle(HWND window_handle)
{
    for (WindowsWindow* window : s_windows)
    {
        if (window->get_native_handle() == static_cast<void*>(window_handle))
            return window;
    }

    return nullptr;
}

} // namespace SE
