/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/CoreTypes.h>

namespace SE
{

struct Color3
{
public:
    ALWAYS_INLINE constexpr Color3()
        : r(0.0F)
        , g(0.0F)
        , b(0.0F)
    {}

    ALWAYS_INLINE constexpr Color3(const Color3& other)
        : r(other.r)
        , g(other.g)
        , b(other.b)
    {}

    ALWAYS_INLINE constexpr Color3& operator=(const Color3& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }

    ALWAYS_INLINE constexpr Color3(float in_r, float in_g, float in_b)
        : r(in_r)
        , g(in_g)
        , b(in_b)
    {}

public:
    NODISCARD ALWAYS_INLINE float* value_ptr() { return &r; }
    NODISCARD ALWAYS_INLINE const float* value_ptr() const { return &r; }

public:
    float r;
    float g;
    float b;
};

struct Color4
{
public:
    ALWAYS_INLINE constexpr Color4()
        : r(0.0F)
        , g(0.0F)
        , b(0.0F)
        , a(0.0F)
    {}

    ALWAYS_INLINE constexpr Color4(const Color4& other)
        : r(other.r)
        , g(other.g)
        , b(other.b)
        , a(other.a)
    {}

    ALWAYS_INLINE constexpr Color4& operator=(const Color4& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }

    ALWAYS_INLINE constexpr Color4(float in_r, float in_g, float in_b, float in_a)
        : r(in_r)
        , g(in_g)
        , b(in_b)
        , a(in_a)
    {}

    ALWAYS_INLINE constexpr Color4(float in_r, float in_g, float in_b)
        : r(in_r)
        , g(in_g)
        , b(in_b)
        , a(1.0F)
    {}

    ALWAYS_INLINE constexpr Color4(Color3 color_3)
        : r(color_3.r)
        , g(color_3.g)
        , b(color_3.b)
        , a(1.0F)
    {}

    ALWAYS_INLINE constexpr Color4& operator=(Color3 color_3)
    {
        r = color_3.r;
        g = color_3.g;
        b = color_3.b;
        a = 1.0F;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE float* value_ptr() { return &r; }
    NODISCARD ALWAYS_INLINE const float* value_ptr() const { return &r; }

public:
    float r;
    float g;
    float b;
    float a;
};

} // namespace SE
