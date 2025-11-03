// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class Math
{
    SE_MAKE_NAMESPACE_CLASS(Math);

public:
    static constexpr float SMALL_NUMBER = 1E-5F;
    static constexpr float KINDA_SMALL_NUMBER = 1E-3F;

    static constexpr float KINDA_BIG_NUMBER = 1E20F;
    static constexpr float BIG_NUMBER = 1E30F;

    static constexpr float PI      = 3.141592654F;
    static constexpr float TWO_PI  = 6.283185307F;
    static constexpr float HALF_PI = 1.570796326F;

public:
    template<typename T>
    NODISCARD ALWAYS_INLINE static T Min(T a, T b)
    {
        return (a < b) ? a : b;
    }

    template<typename T>
    NODISCARD ALWAYS_INLINE static T Max(T a, T b)
    {
        return (a > b) ? a : b;
    }

    template<typename T>
    NODISCARD ALWAYS_INLINE static T Clamp(T value, T minBound, T maxBound)
    {
        SE_ASSERT(minBound <= maxBound);
        return Math::Min<T>(maxBound, Math::Max<T>(minBound, value));
    }

    template<typename T>
    NODISCARD ALWAYS_INLINE static T Abs(T value)
    {
        return (value > 0) ? value : -value;
    }

public:
    NODISCARD ALWAYS_INLINE static bool AreNearlyEqual(float a, float b, float tolerance)
    {
        const float delta = Math::Abs(a - b);
        return (delta <= tolerance);
    }
    
    NODISCARD ALWAYS_INLINE static bool AreNearlyEqual(float a, float b)
    {
        return AreNearlyEqual(a, b, SMALL_NUMBER);
    }

public:
    NODISCARD RUNTIME_API static float Sqrt(float value);

    NODISCARD RUNTIME_API static float Sin(float value);
    NODISCARD RUNTIME_API static float Cos(float value);
    NODISCARD RUNTIME_API static float Tan(float value);

    NODISCARD RUNTIME_API static float Asin(float value);
    NODISCARD RUNTIME_API static float Acos(float value);
    NODISCARD RUNTIME_API static float Atan(float value);

public:
    NODISCARD ALWAYS_INLINE static float ToRad(float degrees)
    {
        constexpr float DEG_TO_RAD = PI / 180.0F;
        return degrees * DEG_TO_RAD;
    }

    NODISCARD ALWAYS_INLINE static float ToDeg(float radians)
    {
        constexpr float RAD_TO_DEG = 180.0F / PI;
        return radians * RAD_TO_DEG;
    }
};

}
