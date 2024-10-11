/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Engine/Application/Event.h>

namespace SE
{

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

} // namespace SE
