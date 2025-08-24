// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/Platform/PlatformTime.h>
#include <Runtime/Core/Time.h>

namespace SE
{

TimeDuration TimeDuration::FromSeconds(float seconds)
{
    const uint64 ticksPerSecond = PlatformTime::GetPerformanceCounterFrequency();
    const double tickCount = (double)seconds * (double)ticksPerSecond;
    TimeDuration duration;
    duration.m_TickCount = (uint64)tickCount;
    return duration;
}

TimeDuration TimeDuration::FromMilliseconds(float milliseconds)
{
    const uint64 ticksPerSecond = PlatformTime::GetPerformanceCounterFrequency();
    const double tickCount = (double)milliseconds * ((double)ticksPerSecond * 1E-3);
    TimeDuration duration;
    duration.m_TickCount = (uint64)tickCount;
    return duration;
}

TimeDuration TimeDuration::FromMicroseconds(float microseconds)
{
    const uint64 ticksPerSecond = PlatformTime::GetPerformanceCounterFrequency();
    const double tickCount = (double)microseconds * ((double)ticksPerSecond * 1E-6);
    TimeDuration duration;
    duration.m_TickCount = (uint64)tickCount;
    return duration;
}

TimeDuration TimeDuration::FromNanoseconds(float nanoseconds)
{
    const uint64 ticksPerSecond = PlatformTime::GetPerformanceCounterFrequency();
    const double tickCount = (double)nanoseconds * ((double)ticksPerSecond * 1E-9);
    TimeDuration duration;
    duration.m_TickCount = (uint64)tickCount;
    return duration;
}

float TimeDuration::ToSeconds() const
{
    const uint64 ticksPerSecond = PlatformTime::GetPerformanceCounterFrequency();
    const double seconds = (double)m_TickCount / (double)ticksPerSecond;
    return (float)seconds;
}

float TimeDuration::ToMilliseconds() const
{
    const uint64 ticksPerSecond = PlatformTime::GetPerformanceCounterFrequency();
    const double milliseconds = (double)m_TickCount / ((double)ticksPerSecond * 1E-3);
    return (float)milliseconds;
}

float TimeDuration::ToMicroseconds() const
{
    const uint64 ticksPerSecond = PlatformTime::GetPerformanceCounterFrequency();
    const double microseconds = (double)m_TickCount / ((double)ticksPerSecond * 1E-6);
    return (float)microseconds;
}

float TimeDuration::ToNanoseconds() const
{
    const uint64 ticksPerSecond = PlatformTime::GetPerformanceCounterFrequency();
    const double nanoseconds = (double)m_TickCount / ((double)ticksPerSecond * 1E-9);
    return (float)nanoseconds;
}

void Timer::Start()
{
    SE_ASSERT(!IsRunning());
    m_StartTickCount = PlatformTime::GetCurrentPerformanceCounter();
    m_EndTickCount = 0;
}
    
void Timer::Stop()
{
    SE_ASSERT(IsRunning());
    m_EndTickCount = PlatformTime::GetCurrentPerformanceCounter();
}

void Timer::Reset()
{
    SE_ASSERT(!IsRunning());
    m_StartTickCount = 0;
    m_EndTickCount = 0;
}

TimeDuration Timer::GetElapsed() const
{
    uint64 endTickCount = m_EndTickCount;
    if (IsRunning())
        endTickCount = PlatformTime::GetCurrentPerformanceCounter();

    const uint64 elapsedTickCount = endTickCount - m_StartTickCount;
    return TimeDuration::FromTicks(elapsedTickCount);
}

bool Timer::IsRunning() const
{
    if (m_StartTickCount == 0)
        return false;
    return (m_EndTickCount == 0);
}

}
