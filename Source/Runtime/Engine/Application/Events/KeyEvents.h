/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Engine/Application/Events/Event.h>
#include <Engine/Application/KeyCode.h>

namespace SE
{

class KeyDownEvent : public Event
{
public:
    NODISCARD ALWAYS_INLINE static EventType get_static_type() { return EventType::KeyDown; }
    virtual ~KeyDownEvent() override = default;

public:
    ALWAYS_INLINE KeyDownEvent(KeyCode key_code, u32 repeat_count)
        : Event(get_static_type())
        , m_key_code(key_code)
        , m_repeat_count(repeat_count)
    {}

    NODISCARD ALWAYS_INLINE KeyCode get_key_code() const { return m_key_code; }
    NODISCARD ALWAYS_INLINE u32 get_repeat_count() const { return m_repeat_count; }

private:
    KeyCode m_key_code;
    u32 m_repeat_count;
};

class KeyUpEvent : public Event
{
public:
    NODISCARD ALWAYS_INLINE static EventType get_static_type() { return EventType::KeyUp; }
    virtual ~KeyUpEvent() override = default;

public:
    ALWAYS_INLINE KeyUpEvent(KeyCode key_code)
        : Event(get_static_type())
        , m_key_code(key_code)
    {}

    NODISCARD ALWAYS_INLINE KeyCode get_key_code() const { return m_key_code; }

private:
    KeyCode m_key_code;
};

} // namespace SE
