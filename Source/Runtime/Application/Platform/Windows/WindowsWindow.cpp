// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Platform/Windows/WindowsWindow.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Log.h>

namespace SE
{

static Vector<WindowsWindow*> s_ActiveWindows;

NODISCARD static RefPtr<WindowsWindow> GetWindowFromNativeHandle(HWND nativeHandle)
{
    for (WindowsWindow* window : s_ActiveWindows)
    {
        if (window != nullptr && window->GetNativeHandle() == nativeHandle)
            return AdoptRef<WindowsWindow>(window);
    }
    return {};
}

static LRESULT WindowsWindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    RefPtr<Window> window = GetWindowFromNativeHandle(windowHandle);
    if (!window.IsValid())
        return DefWindowProcA(windowHandle, message, wParam, lParam);

    switch (message)
    {
        case WM_CLOSE:
        {
            window->Close();
            return 0;
        }

        case WM_SIZE:
        {
            const uint32 newSizeX = LOWORD(lParam);
            const uint32 newSizeY = HIWORD(lParam);
            window->GetOnWindowResizedDelegate().Broadcast(window, newSizeX, newSizeY);
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            const int64 delta = GET_WHEEL_DELTA_WPARAM(wParam);
            window->GetOnMouseWheelScrolledDelegate().Broadcast(window, static_cast<float>(delta) / static_cast<float>(WHEEL_DELTA));
            return 0;
        }
    }

    return DefWindowProcA(windowHandle, message, wParam, lParam);
}

WindowsWindow::WindowsWindow(const WindowInfo& info)
    : m_WindowHandle(nullptr)
    , m_IsRequestedToClose(false)
{
    // Register the window class.
    static bool s_IsWindowClassRegistered = false;
    if (!s_IsWindowClassRegistered)
    {
        WNDCLASSA windowClass     = {};
        windowClass.lpfnWndProc   = WindowsWindowProcedure;
        windowClass.hInstance     = GetModuleHandle(nullptr);
        windowClass.lpszClassName = "ShooterWindowClass";

        RegisterClassA(&windowClass);
        s_IsWindowClassRegistered = true;
    }

    // Determine the window position and size.
    const int windowSizeX     = info.SizeX.ValueOr(static_cast<uint32>(CW_USEDEFAULT));
    const int windowSizeY     = info.SizeY.ValueOr(static_cast<uint32>(CW_USEDEFAULT));
    const int windowPositionX = info.PositionX.ValueOr(CW_USEDEFAULT);
    const int windowPositionY = info.PositionY.ValueOr(CW_USEDEFAULT);

    // Determine the window creation flags.
    DWORD windowStyleFlags = WS_OVERLAPPEDWINDOW;
    DWORD windowShowMode   = SW_SHOW;

    if (info.StartMode == WindowMode::Maximized)
    {
        windowStyleFlags |= WS_MAXIMIZE;
        windowShowMode = SW_MAXIMIZE;
    }
    if (info.StartMode == WindowMode::Minimized)
    {
        windowStyleFlags |= WS_MAXIMIZE;
        windowShowMode = SW_MINIMIZE;
    }

    // Create the window.
    m_WindowHandle = CreateWindowA("ShooterWindowClass", info.Title.Characters(), windowStyleFlags, windowPositionX, windowPositionY, windowSizeX, windowSizeY,
                                   nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    if (m_WindowHandle == nullptr)
    {
        SE_LOG_ERROR("Failed to create window with title '%s'!", info.Title.Characters());
        SE_ASSERT_NOT_REACHED;
        return;
    }
    ShowWindow(m_WindowHandle, windowShowMode);

    if (info.StartMode == WindowMode::Fullscreen)
    {
        EnterFullscreen(WindowMode::Windowed);
    }

    // Add the window to the active window list.
    s_ActiveWindows.Add(this);
}

WindowsWindow::~WindowsWindow()
{
    // Remove the window from the active window list.
    Optional<usize> windowIndex;
    for (usize index = 0; index < s_ActiveWindows.Count(); ++index)
    {
        if (s_ActiveWindows[index] == this)
        {
            windowIndex = index;
            break;
        }
    }
    if (windowIndex.HasValue())
    {
        // NOTE(Traian): Replace the window pointer stored at the given index with the
        // last element in the array and pop the container. While this operation doesn't
        // preserve elements order, it is more performant.
        s_ActiveWindows[*windowIndex] = s_ActiveWindows.Last();
        s_ActiveWindows.PopBack();

        // NOTE(Traian): When no windows are active, no memory is allocated by this container. Since the engine memory management
        // system outlives the windowing manager, all memory will be released before the memory system shutdown.
        s_ActiveWindows.ShrinkToFit();
    }

    DestroyWindow(m_WindowHandle);
    m_WindowHandle = nullptr;
    m_FullscreenState.Clear();
}

uint32 WindowsWindow::GetSizeX() const
{
    RECT windowClientRect = {};
    if (GetClientRect(m_WindowHandle, &windowClientRect))
    {
        const uint32 windowSizeX = windowClientRect.right - windowClientRect.left;
        return windowSizeX;
    }

    return 0;
}

uint32 WindowsWindow::GetSizeY() const
{
    RECT windowClientRect = {};
    if (GetClientRect(m_WindowHandle, &windowClientRect))
    {
        const uint32 windowSizeY = windowClientRect.bottom - windowClientRect.top;
        return windowSizeY;
    }

    return 0;
}

WindowMode WindowsWindow::GetCurrentMode() const
{
    if (m_FullscreenState.HasValue())
        return WindowMode::Fullscreen;

    if (IsZoomed(m_WindowHandle))
        return WindowMode::Maximized;

    if (IsIconic(m_WindowHandle))
        return WindowMode::Minimized;

    return WindowMode::Windowed;
}

void WindowsWindow::SetSize(Optional<uint32> sizeX, Optional<uint32> sizeY)
{
    RECT currentWindowClientRect = {};
    GetClientRect(m_WindowHandle, &currentWindowClientRect);

    // Get the new size of the window client area.
    const uint32 newClientSizeX = sizeX.ValueOr(static_cast<uint32>(currentWindowClientRect.right - currentWindowClientRect.left));
    const uint32 newClientSizeY = sizeY.ValueOr(static_cast<uint32>(currentWindowClientRect.bottom - currentWindowClientRect.top));

    // Get current window style & ex-style.
    const DWORD currentStyle   = GetWindowLong(m_WindowHandle, GWL_STYLE);
    const DWORD currentExStyle = GetWindowLong(m_WindowHandle, GWL_EXSTYLE);

    // Adjust the window rectangle.
    RECT windowRect = { 0, 0, static_cast<LONG>(newClientSizeX), static_cast<LONG>(newClientSizeY) };
    AdjustWindowRectEx(&windowRect, currentStyle, FALSE, currentExStyle);

    // Resize the window to that rectangle.
    SetWindowPos(m_WindowHandle, nullptr, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void WindowsWindow::SetCurrentMode(WindowMode mode)
{
    const WindowMode previousMode = GetCurrentMode();
    if (mode == previousMode)
        return;

    // Handle fullscreen state.
    if (mode == WindowMode::Fullscreen)
    {
        EnterFullscreen(previousMode);
    }
    if (previousMode == WindowMode::Fullscreen)
    {
        ExitFullscreen(mode);
    }

    // Handle non-fullscreen state.
    if (mode == WindowMode::Windowed)
        ShowWindow(m_WindowHandle, SW_RESTORE);

    if (mode == WindowMode::Minimized)
        ShowWindow(m_WindowHandle, SW_MINIMIZE);

    if (mode == WindowMode::Maximized)
        ShowWindow(m_WindowHandle, SW_MAXIMIZE);
}

void WindowsWindow::ProcessEventQueue()
{
    MSG message = {};
    while (PeekMessageA(&message, m_WindowHandle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

void WindowsWindow::Close()
{
    m_IsRequestedToClose = true;
}

void WindowsWindow::EnterFullscreen(WindowMode previousMode)
{
    SE_ASSERT(previousMode != WindowMode::Fullscreen);

    // Get the previous window placement. It will be used to restore the window when exiting fullscreen mode.
    WINDOWPLACEMENT windowPlacement = {};
    GetWindowPlacement(m_WindowHandle, &windowPlacement);
    m_FullscreenState                          = FullscreenState();
    m_FullscreenState->PreviousMode            = previousMode;
    m_FullscreenState->PreviousWindowPlacement = windowPlacement;

    // Remove window decorations.
    SetWindowLong(m_WindowHandle, GWL_STYLE, WS_POPUP | WS_VISIBLE);

    // Make the window fill the entire screen.
    MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
    if (GetMonitorInfo(MonitorFromWindow(m_WindowHandle, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
    {
        // clang-format off
        SetWindowPos(
            m_WindowHandle, HWND_TOP,
            monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
            monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
            SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
        // clang-format on
    }
}

void WindowsWindow::ExitFullscreen(WindowMode newMode)
{
    SE_ASSERT(GetCurrentMode() == WindowMode::Fullscreen);
    SE_ASSERT(newMode != WindowMode::Fullscreen);

    // Restore previous window style
    SetWindowLong(m_WindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

    SetWindowPlacement(m_WindowHandle, &m_FullscreenState->PreviousWindowPlacement);
    SetWindowPos(m_WindowHandle, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

    m_FullscreenState.Clear();
}

} // namespace SE
