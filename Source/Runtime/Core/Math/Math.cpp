// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Math/Math.h>

#include <cmath>

namespace SE
{

float Math::Sqrt(float value)
{
    return std::sqrtf(value);
}

float Math::Sin(float value)
{
    return std::sinf(value);
}

float Math::Cos(float value)
{
    return std::cosf(value);
}

float Math::Tan(float value)
{
    return std::tanf(value);
}

float Math::Asin(float value)
{
    return std::asinf(value);
}

float Math::Acos(float value)
{
    return std::acosf(value);
}

float Math::Atan(float value)
{
    return std::atanf(value);
}

}
