/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/CoreDefines.h>
#include <Core/CoreTypes.h>

#ifdef min
    #undef min
#endif // min
#ifdef max
    #undef max
#endif // max

namespace SE
{
namespace Math
{

/** PI constants. */
static constexpr float pi = 3.1415926535F;
static constexpr float two_pi = 2.0F * pi;
static constexpr float half_pi = 0.5F * pi;
static constexpr float one_over_pi = 1.0F / pi;

/** Numerical constants that express the magnitude of numbers. */
static constexpr float small_number = 1e-3F;
static constexpr float very_small_number = 1e-4F;
static constexpr float large_number = 1e20F;
static constexpr float very_large_number = 1e35F;

template<typename T>
NODISCARD ALWAYS_INLINE static constexpr T abs(T value)
{
    return (value < T(0)) ? -value : value;
}

template<typename T>
NODISCARD ALWAYS_INLINE static constexpr bool are_nearly_equal(T a, T b, T tolerance = small_number)
{
    return (abs(a - b) < tolerance);
}

template<typename T>
NODISCARD ALWAYS_INLINE static constexpr T min(T a, T b)
{
    return (a < b) ? a : b;
}

template<typename T>
NODISCARD ALWAYS_INLINE static constexpr T max(T a, T b)
{
    return (a > b) ? a : b;
}

template<typename T>
NODISCARD ALWAYS_INLINE static constexpr T clamp(T value, T in_min, T in_max)
{
    return min(max(value, in_min), in_max);
}

/** Elementary functions. */
SHOOTER_API NODISCARD float sqrt(float value);
SHOOTER_API NODISCARD float pow(float base, float exponent);
/** The natural logarithm of a number. */
SHOOTER_API NODISCARD float log(float value);
/** The logarithm of a number in base 2. */
SHOOTER_API NODISCARD float log2(float value);
/** The logarithm of a number in base 10. */
SHOOTER_API NODISCARD float log10(float value);

/** Trigonometric functions. */
SHOOTER_API NODISCARD float sin(float value);
SHOOTER_API NODISCARD float cos(float value);
SHOOTER_API NODISCARD void sin_and_cos(float value);
SHOOTER_API NODISCARD float tan(float value);

/** Inverses of trigonometric functions. */
SHOOTER_API NODISCARD float arcsin(float value);
SHOOTER_API NODISCARD float arccos(float value);
SHOOTER_API NODISCARD float arctan(float value);

} // namespace Math
} // namespace SE
