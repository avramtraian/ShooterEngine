// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class PlatformTime
{
    SE_MAKE_NAMESPACE_CLASS(PlatformTime);

public:
    SHOOTER_API NODISCARD static uint64 GetCurrentPerformanceCounter();
    SHOOTER_API NODISCARD static uint64 GetPerformanceCounterFrequency();
};

}
