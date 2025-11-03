// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class PlatformTime
{
    SE_MAKE_NAMESPACE_CLASS(PlatformTime);

public:
    NODISCARD RUNTIME_API static uint64 GetCurrentPerformanceCounter();
    NODISCARD RUNTIME_API static uint64 GetPerformanceCounterFrequency();
};

} // namespace SE
