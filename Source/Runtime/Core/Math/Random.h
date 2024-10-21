/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/CoreTypes.h>

namespace SE
{

class Random
{
public:
    NODISCARD SHOOTER_API static u32 UInt32();
    NODISCARD SHOOTER_API static u64 UInt64();

    NODISCARD SHOOTER_API static u32 UInt32Range(u32 range_min, u32 range_max);
    NODISCARD SHOOTER_API static u64 UInt64Range(u64 range_min, u64 range_max);
    NODISCARD SHOOTER_API static float FloatRange(float range_min, float range_max);
};

} // namespace SE
