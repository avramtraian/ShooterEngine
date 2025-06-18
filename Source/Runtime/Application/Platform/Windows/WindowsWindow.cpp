// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Window.h>
#include <Runtime/Core/Platform/PlatformCoreInclude.h>

#include <unordered_map>

namespace SE
{

struct EventCallbackInfo
{
    EventType               Type     { EventType::Unknown };
    WindowEventCallbackID   ID       { INVALID_WINDOW_EVENT_CALLBACK_ID };
    PFN_WindowEventCallback Callback;
};

struct WindowPlatformData
{
    HWND Handle { nullptr };
    std::unordered_map<WindowEventCallbackID, EventCallbackInfo> EventCallbacks;
    WindowEventCallbackID LastUsedEventCallbackID { 0 };
};

OwnPtr<Window> Window::Create(const WindowInfo& info)
{
    Window* windowInstance = new Window(info);
    if (windowInstance->m_IsInitialized)
        return AdoptOwn<Window>(windowInstance);

    delete windowInstance;
    return {};
}

static std::unordered_map<HWND, Window*> s_ActiveWindowTable;

static LRESULT Win32WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CLOSE:
        {
            if (s_ActiveWindowTable.contains(windowHandle))
            {
                Window& window = *s_ActiveWindowTable.at(windowHandle);
                window.SubmitCloseRequest();
                return 0;
            }

            /* Forward the message handling to the default Win32 layer. */
            break;
        }

        case WM_SIZE:
        {
            if (s_ActiveWindowTable.contains(windowHandle))
            {
                Window& window = *s_ActiveWindowTable.at(windowHandle);
                const uint32 newSizeX = LOWORD(lParam);
                const uint32 newSizeY = HIWORD(lParam);
                window.DispatchEvent(WindowResizedEvent::GetStaticType(), WindowResizedEvent(newSizeX, newSizeY));
                return 0;
            }

            /* Forward the message handling to the default Win32 layer. */
            break;
        }

        case WM_MOUSEWHEEL:
        {
            if (s_ActiveWindowTable.contains(windowHandle))
            {
                Window& window = *s_ActiveWindowTable.at(windowHandle);
                const float scrollOffset = (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
                window.DispatchEvent(MouseWheelScrolledEvent::GetStaticType(), MouseWheelScrolledEvent(scrollOffset));
                return 0;
            }

            /* Forward the message handling to the default Win32 layer. */
            break;
        }
    }

    return DefWindowProcA(windowHandle, message, wParam, lParam);
}

Window::Window(const WindowInfo& info)
    : m_PlatformData(nullptr)
    , m_IsInitialized(false)
    , m_IsRequestedToClose(false)
{
    static bool s_IsWindowClassRegistered = false;
    if (!s_IsWindowClassRegistered)
    {
        WNDCLASSA windowClass = {};
        windowClass.hInstance = GetModuleHandle(nullptr);
        windowClass.lpszClassName = "ShooterWindowClass";
        windowClass.lpfnWndProc = Win32WindowProcedure;

        RegisterClassA(&windowClass);
        s_IsWindowClassRegistered = true;
    }

    DWORD windowStyleFlags = WS_OVERLAPPEDWINDOW;
    int showCommand = 0;

    switch (info.StartMode)
    {
        case WindowMode::Windowed:  windowStyleFlags |= 0;           showCommand = SW_NORMAL;   break;
        case WindowMode::Maximized: windowStyleFlags |= WS_MAXIMIZE; showCommand = SW_MAXIMIZE; break;
        case WindowMode::Minimized: windowStyleFlags |= WS_MINIMIZE; showCommand = SW_MINIMIZE; break;
    }

    const int windowPositionX = info.PositionX.ValueOr(CW_USEDEFAULT);
    const int windowPositionY = info.PositionY.ValueOr(CW_USEDEFAULT);
    const int windowSizeX = info.SizeX.ValueOr(CW_USEDEFAULT);
    const int windowSizeY = info.SizeY.ValueOr(CW_USEDEFAULT);

    /* Create the window. */
    const HWND windowHandle = CreateWindowA(
        "ShooterWindowClass", info.Title.Characters(), windowStyleFlags,
        windowPositionX, windowPositionY, windowSizeX, windowSizeY,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );
    if (windowHandle == nullptr)
        return;

    /* Show the window. */
    ShowWindow(windowHandle, showCommand);

    m_IsInitialized = true;
    m_PlatformData = new WindowPlatformData();
    m_PlatformData->Handle = windowHandle;

    /* Add this window to the active windows table. */
    s_ActiveWindowTable.insert({ windowHandle, this });
}

Window::~Window()
{
    if (!m_IsInitialized)
        return;

    /* Destroy the native window object. */
    DestroyWindow(m_PlatformData->Handle);
    m_PlatformData->Handle = nullptr;

    delete m_PlatformData;
    m_PlatformData = nullptr;
}

void Window::SubmitCloseRequest()
{
    if (!m_IsInitialized)
        return;

    /* Mark the window was waiting to be closed. */
    m_IsRequestedToClose = true;
}

void Window::PumpMessages()
{
    if (!m_IsInitialized)
        return;

    MSG message = {};
    while (PeekMessageA(&message, m_PlatformData->Handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

WindowEventCallbackID Window::AddEventCallback(EventType eventType, PFN_WindowEventCallback pfnCallback)
{
    if (!m_PlatformData)
        return INVALID_WINDOW_EVENT_CALLBACK_ID;
    const WindowEventCallbackID callbackID = m_PlatformData->LastUsedEventCallbackID++;

    EventCallbackInfo callbackInfo = {};
    callbackInfo.Type = eventType;
    callbackInfo.ID = callbackID;
    callbackInfo.Callback = pfnCallback;
    m_PlatformData->EventCallbacks.insert({ callbackID, callbackInfo });

    return callbackID;
}

void Window::RemoveEventCallback(WindowEventCallbackID callbackID)
{
    if (!m_PlatformData)
        return;
    if (!m_PlatformData->EventCallbacks.contains(callbackID))
        return;
    
    /* Remove the callback from the dispatch table. */
    m_PlatformData->EventCallbacks.erase(callbackID);
}

Vector2u Window::GetSize() const
{
    Vector2u windowSize = { 0, 0 };

    if (!m_IsInitialized)
        return windowSize;

    RECT windowRect = {};
    if (GetClientRect(m_PlatformData->Handle, &windowRect))
    {
        windowSize.X = (uint32)(windowRect.right - windowRect.left);
        windowSize.Y = (uint32)(windowRect.bottom - windowRect.top);
    }

    return windowSize;
}

Vector2i Window::GetPosition() const
{
    Vector2i windowPosition = { 0, 0 };

    if (!m_IsInitialized)
        return windowPosition;

    RECT windowRect = {};
    if (GetClientRect(m_PlatformData->Handle, &windowRect))
    {
        windowPosition.X = windowRect.left;
        windowPosition.Y = windowRect.top;
    }

    return windowPosition;
}

void* Window::GetNativeHandle() const
{
    if (!m_IsInitialized)
        return nullptr;
    return m_PlatformData->Handle;
}

void Window::DispatchEvent(EventType eventType, const Event& event)
{
    if (!m_IsInitialized)
        return;

    for (const auto& [callbackID, callbackInfo] : m_PlatformData->EventCallbacks)
    {
        if (callbackInfo.Type == eventType)
            callbackInfo.Callback(*this, event);
    }
}

}
