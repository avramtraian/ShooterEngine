/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Engine/Application/Events/Event.h>
#include <Engine/Application/KeyCode.h>

namespace SE
{

class MouseButtonDownEvent : public Event
{
public:
    NODISCARD ALWAYS_INLINE static EventType get_static_type() { return EventType::MouseButtonDown; }
    virtual ~MouseButtonDownEvent() override = default;

public:
    ALWAYS_INLINE explicit MouseButtonDownEvent(MouseButton mouse_button)
        : Event(get_static_type())
        , m_mouse_button(mouse_button)
    {}

    NODISCARD ALWAYS_INLINE MouseButton get_mouse_button() const { return m_mouse_button; }

private:
    MouseButton m_mouse_button;
};

class MouseButtonUpEvent : public Event
{
public:
    NODISCARD ALWAYS_INLINE static EventType get_static_type() { return EventType::MouseButtonUp; }
    virtual ~MouseButtonUpEvent() override = default;

public:
    ALWAYS_INLINE explicit MouseButtonUpEvent(MouseButton mouse_button)
        : Event(get_static_type())
        , m_mouse_button(mouse_button)
    {}

    NODISCARD ALWAYS_INLINE MouseButton get_mouse_button() const { return m_mouse_button; }

private:
    MouseButton m_mouse_button;
};

class MouseMovedEvent : public Event
{
public:
    NODISCARD ALWAYS_INLINE static EventType get_static_type() { return EventType::MouseMoved; }
    virtual ~MouseMovedEvent() override = default;

public:
    ALWAYS_INLINE MouseMovedEvent(i32 mouse_position_x, i32 mouse_position_y)
        : Event(get_static_type())
        , m_mouse_position_x(mouse_position_x)
        , m_mouse_position_y(mouse_position_y)
    {}

    NODISCARD ALWAYS_INLINE i32 get_mouse_position_x() const { return m_mouse_position_x; }
    NODISCARD ALWAYS_INLINE i32 get_mouse_position_y() const { return m_mouse_position_y; }

private:
    i32 m_mouse_position_x;
    i32 m_mouse_position_y;
};

class MouseWheelScrolledEvent : public Event
{
public:
    NODISCARD ALWAYS_INLINE static EventType get_static_type() { return EventType::MouseWheelScrolled; }
    virtual ~MouseWheelScrolledEvent() override = default;

public:
    ALWAYS_INLINE explicit MouseWheelScrolledEvent(i32 scroll_offset)
        : Event(get_static_type())
        , m_scroll_offset(scroll_offset)
    {}

    NODISCARD ALWAYS_INLINE i32 get_scroll_offset() const { return m_scroll_offset; }

private:
    i32 m_scroll_offset;
};

} // namespace SE
