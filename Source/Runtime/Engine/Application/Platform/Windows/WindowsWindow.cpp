/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Core/Platform/Windows/WindowsHeaders.h>
#include <Engine/Application/Events/KeyEvents.h>
#include <Engine/Application/Events/MouseEvents.h>
#include <Engine/Application/Events/WindowEvents.h>
#include <Engine/Application/Window.h>

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

    // Set the window mode to be the start mode.
    m_window_mode = description.start_mode;

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

// NOTE: Implemented at the bottom of the file because it is a big and ungly switch statement.
static KeyCode win32_translate_key_code(WPARAM key_code);

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
                    const WindowMode old_mode = window->m_window_mode;
                    window->m_window_mode = WindowMode::Minimized;

                    if (window->m_event_callback && old_mode != WindowMode::Minimized)
                    {
                        const WindowModeChangedEvent window_event = WindowModeChangedEvent(old_mode, WindowMode::Minimized);
                        window->m_event_callback(window_event);
                    }

                    // NOTE: Minimizing a window should not cause a resize event to be dispatched.
                    break;
                }

                if (event_data->w_param == SIZE_MAXIMIZED)
                {
                    const WindowMode old_mode = window->m_window_mode;
                    window->m_window_mode = WindowMode::Maximized;

                    if (window->m_event_callback && old_mode != WindowMode::Maximized)
                    {
                        const WindowModeChangedEvent window_event = WindowModeChangedEvent(old_mode, WindowMode::Maximized);
                        window->m_event_callback(window_event);
                    }
                }
                else if (window->m_event_callback && window->m_window_mode != WindowMode::Default)
                {
                    const WindowMode old_mode = window->m_window_mode;
                    const WindowModeChangedEvent window_event = WindowModeChangedEvent(old_mode, WindowMode::Default);
                    window->m_event_callback(window_event);
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
                        const WindowResizedEvent window_event = WindowResizedEvent(window_width, window_height);
                        window->m_event_callback(window_event);
                    }
                }

                return_code = 0;
            }
            break;

            // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-move
            case WM_MOVE:
            {
                const POINTS window_position = MAKEPOINTS(event_data->l_param);
                if (window_position.x != window->get_client_area_position_x() || window_position.y != window->get_client_area_position_y())
                {
                    window->m_client_area_position_x = window_position.x;
                    window->m_client_area_position_y = window_position.y;
                    if (window->m_event_callback)
                    {
                        const WindowMovedEvent window_event = WindowMovedEvent(window_position.x, window_position.y);
                        window->m_event_callback(window_event);
                    }
                }

                return_code = 0;
            }
            break;

            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-syskeydown
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            {
                if (window->m_event_callback)
                {
                    const KeyCode key_code = win32_translate_key_code(event_data->w_param);
                    const u32 repeat_count = event_data->l_param & 0xFFFF;

                    if (key_code != KeyCode::Unknown)
                    {
                        const KeyDownEvent key_event = KeyDownEvent(key_code, repeat_count);
                        window->m_event_callback(key_event);
                    }
                }

                return_code = 0;
            }
            break;

            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keyup
            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-syskeyup
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                if (window->m_event_callback)
                {
                    const KeyCode key_code = win32_translate_key_code(event_data->w_param);
                    if (key_code != KeyCode::Unknown)
                    {
                        const KeyUpEvent key_event = KeyUpEvent(key_code);
                        window->m_event_callback(key_event);
                    }
                }

                return_code = 0;
            }
            break;

            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttondown
            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mbuttondown
            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-rbuttondown
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            {
                if (window->m_event_callback)
                {
                    MouseButton mouse_button = MouseButton::Unknown;
                    if (event_data->message == WM_LBUTTONDOWN)
                        mouse_button = MouseButton::Left;
                    if (event_data->message == WM_MBUTTONDOWN)
                        mouse_button = MouseButton::Middle;
                    if (event_data->message == WM_RBUTTONDOWN)
                        mouse_button = MouseButton::Right;

                    if (mouse_button != MouseButton::Unknown)
                    {
                        const MouseButtonDownEvent mouse_event = MouseButtonDownEvent(mouse_button);
                        window->m_event_callback(mouse_event);
                    }
                }
                return_code = 0;
            }
            break;

            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttonup
            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mbuttonup
            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-rbuttonup
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            {
                if (window->m_event_callback)
                {
                    MouseButton mouse_button = MouseButton::Unknown;
                    if (event_data->message == WM_LBUTTONUP)
                        mouse_button = MouseButton::Left;
                    if (event_data->message == WM_MBUTTONUP)
                        mouse_button = MouseButton::Middle;
                    if (event_data->message == WM_RBUTTONUP)
                        mouse_button = MouseButton::Right;

                    if (mouse_button != MouseButton::Unknown)
                    {
                        const MouseButtonUpEvent mouse_event = MouseButtonUpEvent(mouse_button);
                        window->m_event_callback(mouse_event);
                    }
                }
                return_code = 0;
            }
            break;

            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove
            case WM_MOUSEMOVE:
            {
                if (window->m_event_callback)
                {
                    POINTS mouse_position = MAKEPOINTS(event_data->l_param);
                    // NOTE: The coordinates provieded by the Win32 API have the origin in the top-left corner of the
                    // window client area. However, the engine requires the coordinate system origin to be in the bottom-left corner.
                    mouse_position.y = window->m_client_area_height - mouse_position.y;
                    const MouseMovedEvent mouse_event = MouseMovedEvent(mouse_position.x, mouse_position.y);
                    window->m_event_callback(mouse_event);
                }
            }
            break;

            // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel
            case WM_MOUSEWHEEL:
            {
                if (window->m_event_callback)
                {
                    const i32 wheel_scroll_offset = GET_WHEEL_DELTA_WPARAM(event_data->w_param) / WHEEL_DELTA;
                    if (wheel_scroll_offset != 0)
                    {
                        const MouseWheelScrolledEvent mouse_event = MouseWheelScrolledEvent(wheel_scroll_offset);
                        window->m_event_callback(mouse_event);
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

// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
static KeyCode win32_translate_key_code(WPARAM key_code)
{
    if (0x41 <= key_code && key_code <= 0x5A)
    {
        // Alphabetical letters.
        return static_cast<KeyCode>(static_cast<u32>(KeyCode::A) + (key_code - 0x41));
    }

    if (0x30 <= key_code && key_code <= 0x39)
    {
        // Digits.
        return static_cast<KeyCode>(static_cast<u32>(KeyCode::Zero) + (key_code - 0x30));
    }

    if (VK_NUMPAD0 <= key_code && key_code <= VK_NUMPAD9)
    {
        // Numpad digits.
        return static_cast<KeyCode>(static_cast<u32>(KeyCode::Numpad0) + (key_code - VK_NUMPAD0));
    }

    return KeyCode::Unknown;
}

} // namespace SE
