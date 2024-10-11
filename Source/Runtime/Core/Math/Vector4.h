/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreDefines.h>
#include <Core/CoreTypes.h>

namespace SE
{
namespace Internal
{

template<typename T>
struct Vector4
{
public:
    ALWAYS_INLINE Vector4()
        : x(T(0))
        , y(T(0))
        , z(T(0))
        , w(T(0))
    {}

    ALWAYS_INLINE Vector4(const Vector4& other)
        : x(other.x)
        , y(other.y)
        , z(other.z)
        , w(other.w)
    {}

    ALWAYS_INLINE Vector4(T in_x, T in_y, T in_z, T in_w)
        : x(in_x)
        , y(in_y)
        , z(in_z)
        , w(in_w)
    {}

    ALWAYS_INLINE Vector4(T scalar)
        : x(scalar)
        , y(scalar)
        , z(scalar)
        , w(scalar)
    {}

    ALWAYS_INLINE Vector4& operator=(const Vector4& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

public:
    T x;
    T y;
    T z;
    T w;
};

} // namespace Internal

using Vector4 = Internal::Vector4<float>;
using Vector4d = Internal::Vector4<double>;

} // namespace SE
