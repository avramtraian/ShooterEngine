/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>

namespace SE
{

enum class EventType : u16
{
    KeyDown,
    KeyUp,

    MouseButtonDown,
    MouseButtonUp,
    MouseMoved,
    MouseWheelScrolled,

    WindowMoved,
    WindowResized,
    WindowModeChanged,
};

class Event
{
public:
    virtual ~Event() = default;
    NODISCARD ALWAYS_INLINE EventType get_type() const { return m_type; }

protected:
    ALWAYS_INLINE Event(EventType type)
        : m_type(type)
    {}

private:
    EventType m_type;
};

} // namespace SE
