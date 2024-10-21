/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/MathCore.h>
#include <cmath>

namespace SE::Math
{

//
// Forward the core math functions implementations to the standard library.
// Note that in the future there might be custom implementations for certain functions.
//

// clang-format off

float sqrtf(float x) { return std::sqrtf(x); }
double sqrtd(double x) { return std::sqrt(x); }
float sinf(float x) { return std::sinf(x); }
double sind(double x) { return std::sin(x); }
float cosf(float x) { return std::cosf(x); }
double cosd(double x) { return std::cos(x); }

SinAndCosResultF sin_and_cos_f(float x)
{
	SinAndCosResultF result;
	result.sin = std::sinf(x);
	result.cos = std::cosf(x);
	return result;
}

SinAndCosResultD sin_and_cos_d(double x)
{
	SinAndCosResultD result;
	result.sin = std::sin(x);
	result.cos = std::cos(x);
	return result;
}

// clang-format on

} // namespace SE::Math
