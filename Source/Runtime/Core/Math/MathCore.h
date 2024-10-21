/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>

namespace SE::Math
{

enum class VectorAxis : u8
{
    X = 0,
    Y = 1,
    Z = 2,
    W = 3,
};

template<typename T>
NODISCARD ALWAYS_INLINE constexpr T min(T a, T b)
{
    return (a < b) ? a : b;
}

template<typename T>
NODISCARD ALWAYS_INLINE constexpr T max(T a, T b)
{
    return (a > b) ? a : b;
}

template<typename T>
NODISCARD ALWAYS_INLINE constexpr T clamp(T value, T min_value, T max_value)
{
    return Math::max(Math::min(value, max_value), min_value);
}

} // namespace SE::Math
