// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Platform/Windows/WindowsWindow.h>
#include <Runtime/Core/Containers/Vector.h>

namespace SE
{

Vector<WindowsWindow*> WindowsWindow::s_WindowTable;

WindowsWindow::WindowsWindow()
    : m_NativeHandle(nullptr)
    , m_ShouldClose(false)
{
    /* Add this window to the static window table. */
    s_WindowTable.Add(this);
}

WindowsWindow::~WindowsWindow()
{
    DestroyWindow(m_NativeHandle);
    m_NativeHandle = nullptr;

    SE_ENSURE(s_WindowTable.Contains(this));
    s_WindowTable.RemoveIndexUnordered(s_WindowTable.FindIndexOf(this));
}

bool WindowsWindow::Initialize(const WindowsWindowInfo& info)
{
    static bool s_was_window_class_registered = false;
    if (!s_was_window_class_registered)
    {
        WindowsWindow::Win32RegisterWindowClass();
        s_was_window_class_registered = true;
    }

    DWORD windowStyleFlags = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    switch (info.StartMode)
    {
        case WindowsWindowMode::Windowed:  windowStyleFlags |= 0;           break;
        case WindowsWindowMode::Maximized: windowStyleFlags |= WS_MAXIMIZE; break;
        case WindowsWindowMode::Minimized: windowStyleFlags |= WS_MINIMIZE; break;
        default: break;
    }

    int windowPositionX = info.PositionX.ValueOr(CW_USEDEFAULT);
    int windowPositionY = info.PositionY.ValueOr(CW_USEDEFAULT);
    int windowSizeX = info.SizeX.ValueOr(CW_USEDEFAULT);
    int windowSizeY = info.SizeY.ValueOr(CW_USEDEFAULT);

    m_NativeHandle = CreateWindowA(
        "ShooterWindowClass", "Shooter Game", windowStyleFlags,
        windowPositionX, windowPositionY, windowSizeX, windowSizeY,
        nullptr, nullptr, GetModuleHandleA(nullptr), nullptr
    );

    if (m_NativeHandle == nullptr)
    {
        /* TODO: Query the last error code interface for better error messages. */
        return false;
    }

    return true;
}

void WindowsWindow::PumpMessages()
{
    MSG message = {};
    while (PeekMessageA(&message, m_NativeHandle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

bool WindowsWindow::ShouldClose() const
{
    return m_ShouldClose;
}

uint32 WindowsWindow::GetSizeX() const
{
    if (m_NativeHandle == nullptr)
    {
        return 0;
    }

    /* Get the window client area. */
    RECT window_client_rect = {};
    const BOOL success = GetClientRect(m_NativeHandle, &window_client_rect);
    SE_ENSURE(success > 0);

    return window_client_rect.right - window_client_rect.left;
}

uint32 WindowsWindow::GetSizeY() const
{
    if (m_NativeHandle == nullptr)
    {
        return 0;
    }

    /* Get the window client area. */
    RECT window_client_rect = {};
    const BOOL success = GetClientRect(m_NativeHandle, &window_client_rect);
    SE_ENSURE(success > 0);

    return window_client_rect.bottom - window_client_rect.top;
}

void* WindowsWindow::GetNativeHandle() const
{
    /* The value of the native handle is always zero (nullptr) if the window is not initialized. */
    return m_NativeHandle;
}

void WindowsWindow::Win32RegisterWindowClass()
{
    WNDCLASSA window_class = {};
    window_class.hInstance = GetModuleHandleA(nullptr);
    window_class.lpfnWndProc = WindowsWindow::Win32WindowProcedure;
    window_class.lpszClassName = "ShooterWindowClass";
    RegisterClassA(&window_class);
}

LRESULT WindowsWindow::Win32WindowProcedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    WindowsWindow* window = WindowsWindow::FindWindowFromHandle(window_handle);
    if (window != nullptr)
    {
        switch (message)
        {
            case WM_CLOSE:
            {
                window->m_ShouldClose = true;
                return 0;
            }
        }
    }

    /* Forward the event handling to the default Windows implementation. */
    return DefWindowProcA(window_handle, message, w_param, l_param);
}

WindowsWindow* WindowsWindow::FindWindowFromHandle(HWND native_handle)
{
    for (WindowsWindow* window : s_WindowTable)
    {
        if (window->GetNativeHandle() == native_handle)
        {
            return window;
        }
    }

    /* No window with the specified native handle was found. Returning null pointer. */
    return nullptr;
}

}
