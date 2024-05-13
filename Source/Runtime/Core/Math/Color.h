/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreTypes.h"

namespace SE
{

// Forward declarations.
struct Color3;
struct Color4;

//=============================================================================
// THREE-COMPONENT COLOR.
//=============================================================================

struct Color3
{
public:
    ALWAYS_INLINE constexpr Color3()
        : r(0)
        , g(0)
        , b(0)
    {}

    ALWAYS_INLINE constexpr Color3(const Color3& other)
        : r(other.r)
        , g(other.g)
        , b(other.b)
    {}

    ALWAYS_INLINE constexpr Color3(float in_r, float in_g, float in_b)
        : r(in_r)
        , g(in_g)
        , b(in_b)
    {}

    ALWAYS_INLINE constexpr explicit Color3(float scalar)
        : r(scalar)
        , g(scalar)
        , b(scalar)
    {}

    ALWAYS_INLINE constexpr Color3& operator=(const Color3& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }

    ALWAYS_INLINE constexpr Color3(const Color4& color4);
    ALWAYS_INLINE constexpr Color3& operator=(const Color4& color4);

public:
    union
    {
        float r;
        float red;
        float x;
    };

    union
    {
        float g;
        float green;
        float y;
    };

    union
    {
        float b;
        float blue;
        float z;
    };
};

//=============================================================================
// FOUR-COMPONENT COLOR.
//=============================================================================

struct Color4
{
public:
    ALWAYS_INLINE constexpr Color4()
        : r(0)
        , g(0)
        , b(0)
        , a(0)
    {}

    ALWAYS_INLINE constexpr Color4(const Color4& other)
        : r(other.r)
        , g(other.g)
        , b(other.b)
        , a(other.a)
    {}

    ALWAYS_INLINE constexpr Color4(float in_r, float in_g, float in_b, float in_a = 1.0F)
        : r(in_r)
        , g(in_g)
        , b(in_b)
        , a(in_a)
    {}

    ALWAYS_INLINE constexpr explicit Color4(float scalar)
        : r(scalar)
        , g(scalar)
        , b(scalar)
        , a(1.0F)
    {}

    ALWAYS_INLINE constexpr Color4& operator=(const Color4& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }

    ALWAYS_INLINE constexpr Color4(const Color3& color3);
    ALWAYS_INLINE constexpr Color4(const Color3& color3, float alpha);
    ALWAYS_INLINE constexpr Color4& operator=(const Color3& color3);

public:
    union
    {
        float r;
        float red;
        float x;
    };

    union
    {
        float g;
        float green;
        float y;
    };

    union
    {
        float b;
        float blue;
        float z;
    };

    union
    {
        float a;
        float alpha;
        float w;
    };
};

//=============================================================================
// INLINE FUNCTIONS IMPLEMENTATIONS.
//=============================================================================

constexpr Color3::Color3(const Color4& color4)
    : r(color4.r)
    , g(color4.g)
    , b(color4.b)
{
}

constexpr Color3& Color3::operator=(const Color4& color4)
{
    r = color4.r;
    g = color4.g;
    b = color4.b;
    return *this;
}

constexpr Color4::Color4(const Color3& color3)
    : r(color3.r)
    , g(color3.g)
    , b(color3.b)
    , a(1.0F)
{
}

constexpr Color4::Color4(const Color3& color3, float alpha)
    : r(color3.r)
    , g(color3.g)
    , b(color3.b)
    , a(alpha)
{
}

constexpr Color4& Color4::operator=(const Color3& color3)
{
    r = color3.r;
    g = color3.g;
    b = color3.b;
    a = 1.0F;
    return *this;
}

} // namespace SE