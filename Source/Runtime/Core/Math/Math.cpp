/*
/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/Math.h>

// Almost all of the math utility functions are just wrappers around the
// standard library functions. This however will be changed in the future,
// as we will write custom implementations for more and more functions.
#include <cmath>

namespace SE
{

float sqrt(float value)
{
    return std::sqrtf(value);
}

float pow(float base, float exponent)
{
    return std::powf(base, exponent);
}

float log(float value)
{
    return std::logf(value);
}

float log2(float value)
{
    return std::log2f(value);
}

float log10(float value)
{
    return std::log10f(value);
}

float sin(float value)
{
    return std::sinf(value);
}

float cos(float value)
{
    return std::cosf(value);
}

void sin_and_cos(float value, float& OutSin, float& OutCos)
{
    // In debug mode, this function is not any faster than simply calling Sin and Cos functions
    // one after another. However, when optimizations are enabled, the compiler *might* recognize
    // that we are trying to compute both the sine and cosine, and the calls will be optimized.

    OutSin = std::sinf(value);
    OutCos = std::cosf(value);
}

float tan(float value)
{
    return std::tanf(value);
}

float arcsin(float value)
{
    return std::asinf(value);
}

float arccos(float value)
{
    return std::acosf(value);
}

float arctan(float value)
{
    return std::atanf(value);
}

} // namespace SE
