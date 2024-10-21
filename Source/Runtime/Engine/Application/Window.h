/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/Optional.h>
#include <Core/Containers/OwnPtr.h>
#include <Core/Containers/String.h>
#include <Core/Containers/Vector.h>
#include <Engine/Application/Event.h>

namespace SE
{

enum class WindowMode : u8
{
    Unknown = 0,
    Default,
    Minimized,
    Maximized,
    Fullscreen,
};

struct WindowNativeEventData;

using PFN_WindowEventCallback = void (*)(const Event&);
using PFN_WindowNativeEventCallback = uintptr (*)(const WindowNativeEventData*);

struct WindowDescription
{
    Optional<u32> client_area_width;
    Optional<u32> client_area_height;
    Optional<i32> client_area_position_x;
    Optional<i32> client_area_position_y;

    Optional<String> title;
    WindowMode start_mode { WindowMode::Default };

    PFN_WindowEventCallback event_callback { nullptr };
    PFN_WindowNativeEventCallback native_event_callback { nullptr };
};

struct WindowNativeData;

class Window
{
    SE_MAKE_NONCOPYABLE(Window);
    SE_MAKE_NONMOVABLE(Window);

    friend class OwnPtr<Window>;

public:
    NODISCARD SHOOTER_API static OwnPtr<Window> create(const WindowDescription& description);
    SHOOTER_API void destroy();

public:
    NODISCARD ALWAYS_INLINE u32 get_client_area_width() const { return m_client_area_width; }
    NODISCARD ALWAYS_INLINE u32 get_client_area_height() const { return m_client_area_height; }

    NODISCARD ALWAYS_INLINE u32 get_client_area_position_x() const { return m_client_area_position_x; }
    NODISCARD ALWAYS_INLINE u32 get_client_area_position_y() const { return m_client_area_position_y; }

    NODISCARD ALWAYS_INLINE bool should_close() const { return m_should_close; }

    // Returns the opaque handle of the native window object.
    NODISCARD SHOOTER_API void* get_native_handle() const;

public:
    SHOOTER_API void pump_messages();

private:
    bool initialize(const WindowDescription& description);

    Window() = default;
    ~Window() = default;

private:
    static Vector<Window*> s_created_windows;
    static uintptr window_procedure(const WindowNativeEventData* event_data);

private:
    u32 m_client_area_width { 0 };
    u32 m_client_area_height { 0 };
    i32 m_client_area_position_x { 0 };
    i32 m_client_area_position_y { 0 };

    PFN_WindowEventCallback m_event_callback { nullptr };
    PFN_WindowNativeEventCallback m_native_event_callback { nullptr };

    bool m_should_close { false };
    WindowNativeData* m_native_data { nullptr };
};

} // namespace SE
