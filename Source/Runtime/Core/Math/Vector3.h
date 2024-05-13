/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Math/VectorMath.h"

namespace SE
{
namespace Internal
{

template<typename T>
struct Vector3
{
public:
    NODISCARD ALWAYS_INLINE static Vector3 zero() { return Vector3(0, 0, 0); }
    NODISCARD ALWAYS_INLINE static Vector3 one() { return Vector3(1, 1, 1); }
    NODISCARD ALWAYS_INLINE static Vector3 unit_x() { return Vector3(1, 0, 0); }
    NODISCARD ALWAYS_INLINE static Vector3 unit_y() { return Vector3(0, 1, 0); }
    NODISCARD ALWAYS_INLINE static Vector3 unit_z() { return Vector3(0, 0, 1); }

public:
    ALWAYS_INLINE Vector3()
        : x(T(0))
        , y(T(0))
        , z(T(0))
    {}

    ALWAYS_INLINE Vector3(const Vector3& other)
        : x(other.x)
        , y(other.y)
        , z(other.z)
    {}

    ALWAYS_INLINE Vector3(T in_x, T in_y, T in_z)
        : x(in_x)
        , y(in_y)
        , z(in_z)
    {}

    ALWAYS_INLINE Vector3(T scalar)
        : x(scalar)
        , y(scalar)
        , z(scalar)
    {}

    ALWAYS_INLINE Vector3& operator=(const Vector3& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

public:
    T x;
    T y;
    T z;
};

} // namespace Internal

using Vector3 = Internal::Vector3<float>;
using Vector3d = Internal::Vector3<double>;

} // namespace SE