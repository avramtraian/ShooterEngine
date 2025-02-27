// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Platform/Windows/WindowsWindow.h>
#include <Runtime/Core/Containers/Vector.h>

namespace SE
{

Vector<WindowsWindow*> WindowsWindow::s_window_table;

WindowsWindow::WindowsWindow()
    : m_native_handle(nullptr)
    , m_should_close(false)
{
    /* Add this window to the static window table. */
    s_window_table.add(this);
}

WindowsWindow::~WindowsWindow()
{
    DestroyWindow(m_native_handle);
    m_native_handle = nullptr;

    SE_ENSURE(s_window_table.contains(this));
    s_window_table.remove_index_unordered(s_window_table.find_index_of(this));
}

bool WindowsWindow::initialize(const WindowsWindowInfo& info)
{
    static bool s_was_window_class_registered = false;
    if (!s_was_window_class_registered)
    {
        WindowsWindow::win32_register_window_class();
        s_was_window_class_registered = true;
    }

    DWORD window_style_flags = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    switch (info.mode)
    {
        case WindowsWindowMode::Windowed:  window_style_flags |= 0;           break;
        case WindowsWindowMode::Maximized: window_style_flags |= WS_MAXIMIZE; break;
        case WindowsWindowMode::Minimized: window_style_flags |= WS_MINIMIZE; break;
        default: break;
    }

    int window_position_x = CW_USEDEFAULT;
    int window_position_y = CW_USEDEFAULT;
    int window_size_x = info.size_x.value_or(CW_USEDEFAULT);
    int window_size_y = info.size_y.value_or(CW_USEDEFAULT);

    m_native_handle = CreateWindowA(
        "ShooterWindowClass", "Shooter Game", window_style_flags,
        window_position_x, window_position_y, window_size_x, window_size_y,
        nullptr, nullptr, GetModuleHandleA(nullptr), nullptr
    );

    if (m_native_handle == nullptr)
    {
        /* TODO: Query the last error code interface for better error messages. */
        return false;
    }

    return true;
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

bool WindowsWindow::should_close() const
{
    return m_should_close;
}

u32 WindowsWindow::get_size_x() const
{
    if (m_native_handle == nullptr)
    {
        return 0;
    }

    /* Get the window client area. */
    RECT window_client_rect = {};
    const BOOL success = GetClientRect(m_native_handle, &window_client_rect);
    SE_ENSURE(success > 0);

    return window_client_rect.right - window_client_rect.left;
}

u32 WindowsWindow::get_size_y() const
{
    if (m_native_handle == nullptr)
    {
        return 0;
    }

    /* Get the window client area. */
    RECT window_client_rect = {};
    const BOOL success = GetClientRect(m_native_handle, &window_client_rect);
    SE_ENSURE(success > 0);

    return window_client_rect.bottom - window_client_rect.top;
}

void* WindowsWindow::get_native_handle() const
{
    /* The value of the native handle is always zero (nullptr) if the window is not initialized. */
    return m_native_handle;
}

void WindowsWindow::win32_register_window_class()
{
    WNDCLASSA window_class = {};
    window_class.hInstance = GetModuleHandleA(nullptr);
    window_class.lpfnWndProc = WindowsWindow::win32_window_procedure;
    window_class.lpszClassName = "ShooterWindowClass";
    RegisterClassA(&window_class);
}

LRESULT WindowsWindow::win32_window_procedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    WindowsWindow* window = WindowsWindow::find_window_from_handle(window_handle);
    if (window != nullptr)
    {
        switch (message)
        {
            case WM_CLOSE:
            {
                window->m_should_close = true;
                return 0;
            }
        }
    }

    /* Forward the event handling to the default Windows implementation. */
    return DefWindowProcA(window_handle, message, w_param, l_param);
}

WindowsWindow* WindowsWindow::find_window_from_handle(HWND native_handle)
{
    for (WindowsWindow* window : s_window_table)
    {
        if (window->get_native_handle() == native_handle)
        {
            return window;
        }
    }

    /* No window with the specified native handle was found. Returning null pointer. */
    return nullptr;
}

}
