// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Platform/PlatformTime.h>
#include <Runtime/Core/Platform/PlatformCoreInclude.h>

namespace SE
{

uint64 PlatformTime::GetCurrentPerformanceCounter()
{
    LARGE_INTEGER performanceCounter = {};
    QueryPerformanceCounter(&performanceCounter);
    return performanceCounter.QuadPart;
}
    
uint64 PlatformTime::GetPerformanceCounterFrequency()
{
    static uint64 s_PerformanceCounterFrequency = 0;
    if (s_PerformanceCounterFrequency == 0)
    {
        LARGE_INTEGER frequency = {};
        QueryPerformanceFrequency(&frequency);
        s_PerformanceCounterFrequency = frequency.QuadPart;
    }

    return s_PerformanceCounterFrequency;
}

}
