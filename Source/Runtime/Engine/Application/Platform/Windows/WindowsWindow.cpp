/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Engine/Application/Platform/Windows/WindowsWindow.h"
#include "Engine/Application/WindowEvent.h"
#include "Engine/Engine.h"

namespace SE
{

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

WindowsWindow::~WindowsWindow()
{
    DestroyWindow(m_native_handle);
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

#define FIND_WINDOW_BY_NATIVE_HANDLE(variable_name)                                  \
    Window* uncasted_window = g_engine->find_window_by_native_handle(window_handle); \
    if (!uncasted_window)                                                            \
        break;                                                                       \
    WindowsWindow& variable_name = static_cast<WindowsWindow&>(*uncasted_window);

LRESULT WindowsWindow::window_procedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message)
    {
        case WM_QUIT:
        case WM_CLOSE:
        {
            FIND_WINDOW_BY_NATIVE_HANDLE(window);
            window.m_should_close = true;
            return 0;
        }

        case WM_SIZE:
        {
            FIND_WINDOW_BY_NATIVE_HANDLE(window);

            u32 new_width = LOWORD(l_param);
            u32 new_height = HIWORD(l_param);

            if (window.m_client_area_width != new_width || window.m_client_area_height != new_height)
            {
                if (new_width == 0 || new_height == 0)
                {
                    // The window has been minimized. We don't want to propagate window resized events,
                    // nor update the window dimensions.
                    return 0;
                }

                window.m_client_area_width = new_width;
                window.m_client_area_height = new_height;

                if (window.m_event_callback)
                {
                    WindowResizedEvent window_event = WindowResizedEvent(new_width, new_height);
                    window.m_event_callback(window_event);
                }
            }

            return 0;
        }

        case WM_MOVE:
        {
            FIND_WINDOW_BY_NATIVE_HANDLE(window);

            POINTS new_position = MAKEPOINTS(l_param);

            if (window.m_client_area_position_x != new_position.x ||
                window.m_client_area_position_y != new_position.y)
            {
                window.m_client_area_position_x = new_position.x;
                window.m_client_area_position_y = new_position.y;
                // TODO: Propagate window position changed event.
            }

            return 0;
        }
    }

    return DefWindowProcA(window_handle, message, w_param, l_param);
}

#undef FIND_WINDOW_BY_NATIVE_HANDLE

} // namespace SE
