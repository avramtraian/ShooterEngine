/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Core/Platform/Windows/WindowsHeaders.h>
#include <Engine/Application/Window.h>
#include <Engine/Application/WindowEvent.h>

namespace SE
{

Vector<Window*> Window::s_created_windows;

struct WindowNativeData
{
    HWND handle { nullptr };
};

struct WindowNativeEventData
{
    HWND window_handle;
    UINT message;
    WPARAM w_param;
    LPARAM l_param;
};

OwnPtr<Window> Window::create(const WindowDescription& description)
{
    Window* window = new Window();
    if (!window->initialize(description))
        return {};

    return adopt_own(window);
}

static void win32_register_window_class(WNDPROC window_procedure)
{
    static bool s_is_class_registered = false;
    if (!s_is_class_registered)
    {
        WNDCLASSA window_class = {};
        window_class.hInstance = GetModuleHandle(nullptr);
        window_class.lpfnWndProc = window_procedure;
        window_class.lpszClassName = "ShooterWindowClass";
        RegisterClassA(&window_class);

        s_is_class_registered = true;
    }
}

bool Window::initialize(const WindowDescription& description)
{
    if (m_native_data)
    {
        SE_LOG_ERROR("The window has already been initialized!");
        return false;
    }

    s_created_windows.add(this);
    m_native_data = new WindowNativeData();

    const WNDPROC window_procedure_function = [](HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param) -> LRESULT
    {
        WindowNativeEventData event_data = {};
        event_data.window_handle = window_handle;
        event_data.message = message;
        event_data.w_param = w_param;
        event_data.l_param = l_param;
        return static_cast<LRESULT>(Window::window_procedure(&event_data));
    };

    const String window_title = description.title.value_or("Untitled Window"sv);
    const int window_position_x = description.client_area_position_x.value_or(CW_USEDEFAULT);
    const int window_position_y = description.client_area_position_y.value_or(CW_USEDEFAULT);
    const int window_width = description.client_area_width.value_or(CW_USEDEFAULT);
    const int window_height = description.client_area_height.value_or(CW_USEDEFAULT);

    DWORD window_style_flags = 0;
    window_style_flags |= WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE;

    win32_register_window_class(window_procedure_function);
    m_native_data->handle = CreateWindowA(
        "ShooterWindowClass",
        window_title.characters(),
        window_style_flags,
        window_position_x,
        window_position_y,
        window_width,
        window_height,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (m_native_data->handle == nullptr)
        return false;

    // Set the window client area.
    RECT window_client_area = {};
    GetClientRect(m_native_data->handle, &window_client_area);

    m_client_area_width = window_client_area.right - window_client_area.left;
    m_client_area_height = window_client_area.bottom - window_client_area.top;

    // TODO: The Win32 API considers the coordinate origin the top-left corner of the client rect.
    // However, the engine generally considers the origin to be in the bottom-left corner. Make
    // behaviour consistent.
    m_client_area_position_x = window_client_area.left;
    m_client_area_position_y = window_client_area.top;

    // Set the event callback functions.
    m_event_callback = description.event_callback;
    m_native_event_callback = description.native_event_callback;

    return true;
}

void Window::destroy()
{
    if (!m_native_data)
    {
        // The window has already been destroyed.
        return;
    }

    DestroyWindow(m_native_data->handle);
    m_native_data->handle = nullptr;

    delete m_native_data;
    m_native_data = nullptr;

    u32 window_index = 0;
    for (window_index = 0; window_index < s_created_windows.count(); ++window_index)
    {
        if (s_created_windows[window_index] == this)
            break;
    }

    SE_ASSERT(window_index < s_created_windows.count());
    s_created_windows.remove_unordered(window_index);
}

void* Window::get_native_handle() const
{
    SE_ASSERT(m_native_data);
    return static_cast<void*>(m_native_data->handle);
}

void Window::pump_messages()
{
    MSG message = {};
    while (PeekMessageA(&message, m_native_data->handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

uintptr Window::window_procedure(const WindowNativeEventData* event_data)
{
    Window* window = nullptr;
    for (Window* created_window : s_created_windows)
    {
        if (created_window->get_native_handle() == event_data->window_handle)
        {
            window = created_window;
            break;
        }
    }

    if (window != nullptr)
    {
        Optional<uintptr> return_code;

        switch (event_data->message)
        {
            // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-quit
            // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-destroy
            case WM_QUIT:
            case WM_DESTROY:
            {
                window->m_should_close = true;
                return_code = 0;
            }
            break;

            // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-size
            case WM_SIZE:
            {
                if (event_data->w_param == SIZE_MINIMIZED)
                {
                    // NOTE: Minimizing a window should not cause a resize event to be dispatched.
                    break;
                }

                // These are the only allowed values.
                SE_ASSERT(event_data->w_param == SIZE_MAXIMIZED || event_data->w_param == SIZE_RESTORED);

                const UINT window_width = LOWORD(event_data->l_param);
                const UINT window_height = HIWORD(event_data->l_param);
                if (window_width != window->get_client_area_width() || window_height != window->get_client_area_height())
                {
                    window->m_client_area_width = window_width;
                    window->m_client_area_height = window_height;
                    if (window->m_event_callback)
                    {
                        WindowResizedEvent window_event = WindowResizedEvent(window_width, window_height);
                        window->m_event_callback(window_event);
                    }
                }

                return_code = 0;
            }
            break;
        }

        if (window->m_native_event_callback)
            window->m_native_event_callback(event_data);
        
        if (return_code.has_value())
            return return_code.value();
    }

    // Forward the event handling to the default window procedure provided by the Win32 API.
    return DefWindowProcA(event_data->window_handle, event_data->message, event_data->w_param, event_data->l_param);
}

} // namespace SE
