// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Math/Math.h>

#include <cmath>

namespace SE
{

float Math::Sqrt(float value)
{
    return std::sqrt(value);
}

float Math::Sin(float value)
{
    return std::sin(value);
}

float Math::Cos(float value)
{
    return std::cos(value);
}

float Math::Tan(float value)
{
    return std::tan(value);
}

float Math::Asin(float value)
{
    return std::asin(value);
}

float Math::Acos(float value)
{
    return std::acos(value);
}

float Math::Atan(float value)
{
    return std::atan(value);
}

} // namespace SE
