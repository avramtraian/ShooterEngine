// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class TimeDuration
{
public:
    NODISCARD FORCEINLINE static TimeDuration FromTicks(uint64 tickCount)
    {
        TimeDuration duration;
        duration.m_TickCount = tickCount;
        return duration;
    }

    NODISCARD SHOOTER_API static TimeDuration FromSeconds(float seconds);
    NODISCARD SHOOTER_API static TimeDuration FromMilliseconds(float milliseconds);
    NODISCARD SHOOTER_API static TimeDuration FromMicroseconds(float microseconds);
    NODISCARD SHOOTER_API static TimeDuration FromNanoseconds(float nanoseconds);

public:
    TimeDuration() = default;

    TimeDuration(const TimeDuration&) = default;
    TimeDuration& operator=(const TimeDuration&) = default;
    TimeDuration(TimeDuration&&) noexcept = default;
    TimeDuration& operator=(TimeDuration&&) noexcept = default;

public:
    NODISCARD SHOOTER_API float ToSeconds() const;
    NODISCARD SHOOTER_API float ToMilliseconds() const;
    NODISCARD SHOOTER_API float ToMicroseconds() const;
    NODISCARD SHOOTER_API float ToNanoseconds() const;

public:
    NODISCARD FORCEINLINE bool operator==(const TimeDuration& other) const { return (m_TickCount == other.m_TickCount); }
    NODISCARD FORCEINLINE bool operator!=(const TimeDuration& other) const { return (m_TickCount != other.m_TickCount); }
    NODISCARD FORCEINLINE bool operator>(const TimeDuration& other) const { return (m_TickCount > other.m_TickCount); }
    NODISCARD FORCEINLINE bool operator<(const TimeDuration& other) const { return (m_TickCount < other.m_TickCount); }
    NODISCARD FORCEINLINE bool operator>=(const TimeDuration& other) const { return (m_TickCount >= other.m_TickCount); }
    NODISCARD FORCEINLINE bool operator<=(const TimeDuration& other) const { return (m_TickCount <= other.m_TickCount); }

private:
    uint64 m_TickCount { 0 };
};

class Timer
{
public:
    Timer() = default;

    Timer(const Timer&) = default;
    Timer& operator=(const Timer&) = default;
    Timer(Timer&&) noexcept = default;
    Timer& operator=(Timer&&) noexcept = default;

    SHOOTER_API void Start();
    SHOOTER_API void Stop();
    SHOOTER_API void Reset();
    
    NODISCARD SHOOTER_API TimeDuration GetElapsed() const;
    NODISCARD SHOOTER_API bool IsRunning() const;

private:
    uint64 m_StartTickCount { 0 };
    uint64 m_EndTickCount { 0 };
};

}
