// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Core/Containers/String.h>
#include <Runtime/Core/Math/Vector.h>

namespace SE
{

enum class WindowMode
{
    Windowed,
    Maximized,
    Minimized,
};

class WindowInfo
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
    inline WindowInfo& SetStartMode  (WindowMode value) { StartMode = value;            return *this; }
    inline WindowInfo& SetFullscreen (bool value)       { Fullscreen = value;           return *this; }
    inline WindowInfo& SetTitle      (StringView value) { Title = value;                return *this; }
    inline WindowInfo& SetSize(uint32 x, uint32 y)      { SizeX = x; SizeY = y;         return *this; }
    inline WindowInfo& SetPosition(int32 x, int32 y)    { PositionX = x; PositionY = y; return *this; }
};

// Forward declaration to the internal data structure of a window.
struct WindowPlatformData;

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

    NODISCARD SHOOTER_API Vector2u GetSize() const;
    NODISCARD FORCEINLINE uint32 GetSizeX() const { return GetSize().X; }
    NODISCARD FORCEINLINE uint32 GetSizeY() const { return GetSize().Y; }

    NODISCARD SHOOTER_API Vector2i GetPosition() const;
    NODISCARD FORCEINLINE int32 GetPositionX() const { return GetSize().X; }
    NODISCARD FORCEINLINE int32 GetPositionY() const { return GetSize().Y; }

    NODISCARD SHOOTER_API void* GetNativeHandle() const;

    NODISCARD FORCEINLINE WindowPlatformData* GetPlatformData() { return m_PlatformData; }
    NODISCARD FORCEINLINE const WindowPlatformData* GetPlatformData() const { return m_PlatformData; }

private:
    WindowPlatformData* m_PlatformData;
    bool m_IsInitialized;
    bool m_IsRequestedToClose;
};

}
