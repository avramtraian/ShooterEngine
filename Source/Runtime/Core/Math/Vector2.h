/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace SE
{
namespace Internal
{

template<typename T>
struct Vector2
{
public:
    NODISCARD ALWAYS_INLINE static Vector2 zero() { return Vector2(T(0), T(0)); }
    NODISCARD ALWAYS_INLINE static Vector2 unit_x() { return Vector2(T(1), T(0)); }
    NODISCARD ALWAYS_INLINE static Vector2 unit_y() { return Vector2(T(0), T(1)); }

public:
    ALWAYS_INLINE Vector2()
        : x(T(0))
        , y(T(0))
    {}

    ALWAYS_INLINE Vector2(const Vector2& other)
        : x(other.x)
        , y(other.y)
    {}

    ALWAYS_INLINE Vector2(T in_x, T in_y)
        : x(in_x)
        , y(in_y)
    {}

    ALWAYS_INLINE Vector2(T scalar)
        : x(scalar)
        , y(scalar)
    {}

    ALWAYS_INLINE Vector2& operator=(const Vector2& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

public:
    T x;
    T y;
};

} // namespace Internal

using Vector2 = Internal::Vector2<float>;
using Vector2d = Internal::Vector2<double>;

} // namespace SE