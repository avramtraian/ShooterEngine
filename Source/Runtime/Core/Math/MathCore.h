/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
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

// clang-format off

NODISCARD SHOOTER_API float sqrtf(float x);
NODISCARD SHOOTER_API double sqrtd(double x);

NODISCARD ALWAYS_INLINE float sqrt(float x) { return Math::sqrtf(x); }
NODISCARD ALWAYS_INLINE double sqrt(double x) { return Math::sqrtd(x); }

NODISCARD SHOOTER_API float sinf(float x);
NODISCARD SHOOTER_API double sind(double x);

NODISCARD ALWAYS_INLINE float sin(float x) { return Math::sinf(x); }
NODISCARD ALWAYS_INLINE double sin(double x) { return Math::sind(x); }

NODISCARD SHOOTER_API float cosf(float x);
NODISCARD SHOOTER_API double cosd(double x);

NODISCARD ALWAYS_INLINE float cos(float x) { return Math::cosf(x); }
NODISCARD ALWAYS_INLINE double cos(double x) { return Math::cosd(x); }

template<typename T>
struct SinAndCosResult
{
    T sin;
    T cos;
};

using SinAndCosResultF = SinAndCosResult<float>;
using SinAndCosResultD = SinAndCosResult<double>;

NODISCARD SHOOTER_API SinAndCosResultF sin_and_cos_f(float x);
NODISCARD SHOOTER_API SinAndCosResultD sin_and_cos_d(double x);

NODISCARD ALWAYS_INLINE SinAndCosResultF sin_and_cos(float x) { return Math::sin_and_cos_f(x); } 
NODISCARD ALWAYS_INLINE SinAndCosResultD sin_and_cos(double x) { return Math::sin_and_cos_d(x); } 

// clang-format on

} // namespace SE::Math
