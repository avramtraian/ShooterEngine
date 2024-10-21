/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Engine/Application/Events/Event.h>
#include <Engine/Application/Window.h>

namespace SE
{

class WindowMovedEvent : public Event
{
public:
    NODISCARD ALWAYS_INLINE static EventType get_static_type() { return EventType::WindowMoved; }
    virtual ~WindowMovedEvent() override = default;

public:
    ALWAYS_INLINE WindowMovedEvent(i32 client_position_x, i32 client_position_y)
        : Event(get_static_type())
        , m_client_position_x(client_position_x)
        , m_client_position_y(client_position_y)
    {}

    NODISCARD ALWAYS_INLINE i32 get_client_position_x() const { return m_client_position_x; }
    NODISCARD ALWAYS_INLINE i32 get_client_position_y() const { return m_client_position_y; }

private:
    i32 m_client_position_x;
    i32 m_client_position_y;
};

class WindowResizedEvent : public Event
{
public:
    NODISCARD ALWAYS_INLINE static EventType get_static_type() { return EventType::WindowResized; }
    virtual ~WindowResizedEvent() override = default;

public:
    ALWAYS_INLINE WindowResizedEvent(u32 client_width, u32 client_height)
        : Event(get_static_type())
        , m_client_width(client_width)
        , m_client_height(client_height)
    {}

    NODISCARD ALWAYS_INLINE u32 get_client_width() const { return m_client_width; }
    NODISCARD ALWAYS_INLINE u32 get_client_height() const { return m_client_height; }

private:
    u32 m_client_width;
    u32 m_client_height;
};

class WindowModeChangedEvent : public Event
{
public:
    NODISCARD ALWAYS_INLINE static EventType get_static_type() { return EventType::WindowModeChanged; }
    virtual ~WindowModeChangedEvent() override = default;

public:
    ALWAYS_INLINE WindowModeChangedEvent(WindowMode old_window_mode, WindowMode new_window_mode)
        : Event(get_static_type())
        , m_old_window_mode(old_window_mode)
        , m_new_window_mode(new_window_mode)
    {}

    NODISCARD ALWAYS_INLINE WindowMode get_old_window_mode() const { return m_old_window_mode; }
    NODISCARD ALWAYS_INLINE WindowMode get_new_window_mode() const { return m_new_window_mode; }

private:
    WindowMode m_old_window_mode;
    WindowMode m_new_window_mode;
};

} // namespace SE
