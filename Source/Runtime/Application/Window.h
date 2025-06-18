// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Events.h>
#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Core/Containers/String.h>
#include <Runtime/Core/Math/Vector.h>

#include <functional>

namespace SE
{

// Forward declarations.
class Event;
class Window;
struct WindowInfo;

enum class WindowMode
{
    Windowed,
    Maximized,
    Minimized,
};

struct WindowInfo
{
public:
    WindowMode       StartMode  { WindowMode::Windowed };
    bool             Fullscreen { false };
    String           Title      { VIEW("Unnamed Shooter Window") };
    Optional<uint32> SizeX;
    Optional<uint32> SizeY;
    Optional<uint32> PositionX;
    Optional<uint32> PositionY;

public:
    inline WindowInfo& SetStartMode  (WindowMode value)   { StartMode = value;            return *this; }
    inline WindowInfo& SetFullscreen (bool value)         { Fullscreen = value;           return *this; }
    inline WindowInfo& SetTitle      (StringView value)   { Title = value;                return *this; }
    inline WindowInfo& SetSize       (uint32 x, uint32 y) { SizeX = x; SizeY = y;         return *this; }
    inline WindowInfo& SetPosition   (int32 x, int32 y)   { PositionX = x; PositionY = y; return *this; }
};

// Forward declaration to the internal data structure of a window.
struct WindowPlatformData;

using PFN_WindowEventCallback = std::function<void(const class Window&, const class Event&)>;
using WindowEventCallbackID = uint32;
static constexpr WindowEventCallbackID INVALID_WINDOW_EVENT_CALLBACK_ID = -1;

class Window
{
    SE_MAKE_NONCOPYABLE(Window);
    SE_MAKE_NONMOVABLE(Window);

public:
    SHOOTER_API static OwnPtr<Window> Create(const WindowInfo& info);

    SHOOTER_API Window(const WindowInfo& info);
    SHOOTER_API ~Window();

    NODISCARD FORCEINLINE bool IsRequestedToClose() const { return m_IsRequestedToClose; }
    SHOOTER_API void SubmitCloseRequest();
    SHOOTER_API void PumpMessages();

    SHOOTER_API WindowEventCallbackID AddEventCallback(EventType eventType, PFN_WindowEventCallback pfnCallback);
    SHOOTER_API void RemoveEventCallback(WindowEventCallbackID callbackID);

    NODISCARD SHOOTER_API Vector2u GetSize() const;
    NODISCARD FORCEINLINE uint32 GetSizeX() const { return GetSize().X; }
    NODISCARD FORCEINLINE uint32 GetSizeY() const { return GetSize().Y; }

    NODISCARD SHOOTER_API Vector2i GetPosition() const;
    NODISCARD FORCEINLINE int32 GetPositionX() const { return GetSize().X; }
    NODISCARD FORCEINLINE int32 GetPositionY() const { return GetSize().Y; }

    NODISCARD SHOOTER_API void* GetNativeHandle() const;

    NODISCARD FORCEINLINE WindowPlatformData* GetPlatformData() { return m_PlatformData; }
    NODISCARD FORCEINLINE const WindowPlatformData* GetPlatformData() const { return m_PlatformData; }

    // INTERNAL USE ONLY

    void DispatchEvent(EventType eventType, const Event& event);

private:
    WindowPlatformData* m_PlatformData;
    bool m_IsInitialized;
    bool m_IsRequestedToClose;
};

}
