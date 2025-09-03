// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class Random
{
public:
    NODISCARD SHOOTER_API static uint32 UInt32();
    NODISCARD SHOOTER_API static uint64 UInt64();

    NODISCARD SHOOTER_API static uint32 UInt32Range(uint32 range_min, uint32 range_max);
    NODISCARD SHOOTER_API static uint64 UInt64Range(uint64 range_min, uint64 range_max);
    NODISCARD SHOOTER_API static float FloatRange(float range_min, float range_max);
};

}
