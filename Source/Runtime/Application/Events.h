// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Core/Math/Vector.h>

namespace SE
{

enum class EventType : uint8
{
    Unknown = 0,

    WindowResized,
    WindowMoved,
    WindowClosed,

    MouseMoved,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseWheelScrolled,

    KeyPressed,
    KeyReleased,

    MaxEnumValue,
};

class Event
{
    SE_MAKE_NONCOPYABLE(Event);
    SE_MAKE_NONMOVABLE(Event);

public:
    Event(EventType type) : m_Type(type) {}
    virtual ~Event() = default;

    NODISCARD FORCEINLINE EventType GetType() const { return m_Type; }

private:
    EventType m_Type;
};

class WindowResizedEvent : public Event
{
public:
    NODISCARD FORCEINLINE static constexpr EventType GetStaticType() { return EventType::WindowResized; }

public:
    WindowResizedEvent(uint32 newSizeX, uint32 newSizeY)
        : Event(GetStaticType())
        , m_NewSizeX(newSizeX)
        , m_NewSizeY(newSizeY)
    {}

    virtual ~WindowResizedEvent() override = default;

    NODISCARD FORCEINLINE uint32 GetNewSizeX() const { return m_NewSizeX; }
    NODISCARD FORCEINLINE uint32 GetNewSizeY() const { return m_NewSizeY; }
    NODISCARD FORCEINLINE Vector2u GetNewSize() const { return Vector2u(m_NewSizeX, m_NewSizeY); }

private:
    uint32 m_NewSizeX;
    uint32 m_NewSizeY;
};

class MouseWheelScrolledEvent : public Event
{
public:
    NODISCARD FORCEINLINE static constexpr EventType GetStaticType() { return EventType::MouseWheelScrolled; }

public:
    MouseWheelScrolledEvent(float scrollOffset)
        : Event(GetStaticType())
        , m_ScrollOffset(scrollOffset)
    {}

    virtual ~MouseWheelScrolledEvent() override = default;

    NODISCARD FORCEINLINE float GetScrollOffset() const { return m_ScrollOffset; }

private:
    float m_ScrollOffset;
};

}
