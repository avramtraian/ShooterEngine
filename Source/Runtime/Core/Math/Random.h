// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class Random
{
public:
    NODISCARD SHOOTER_API static u32 uint_32();
    NODISCARD SHOOTER_API static u64 uint_64();

    NODISCARD SHOOTER_API static u32 uint_32_range(u32 range_min, u32 range_max);
    NODISCARD SHOOTER_API static u64 uint_64_range(u64 range_min, u64 range_max);
    NODISCARD SHOOTER_API static float float_range(float range_min, float range_max);
};

} // namespace SE
