/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Math/Vector4.h"

namespace SE
{
namespace Internal
{

template<typename T>
struct Matrix4
{
public:
    NODISCARD ALWAYS_INLINE static Matrix4 identity()
    {
        return Matrix4
        (
            { T(1), T(0), T(0), T(0) },
            { T(0), T(1), T(0), T(0) },
            { T(0), T(0), T(1), T(0) },
            { T(0), T(0), T(0), T(1) }
        );
    }

public:
    // Initializes to the null matrix (all elements are zero).
    ALWAYS_INLINE Matrix4()
        : rows{}
    {}

    ALWAYS_INLINE Matrix4(const Matrix4& other)
        : rows{ other.rows[0], other.rows[1], other.rows[2], other.rows[3] }
    {}

    ALWAYS_INLINE Matrix4(const Vector4<T>& row_x, const Vector4<T>& row_y, const Vector4<T>& row_z, const Vector4<T>& row_w)
        : rows{ row_x, row_y, row_z, row_w }
    {}

    ALWAYS_INLINE Matrix4& operator=(const Matrix4& other)
    {
        rows[0] = other.rows[0];
        rows[1] = other.rows[1];
        rows[2] = other.rows[2];
        rows[3] = other.rows[3];
        return *this;
    }

public:
    union
    {
        Vector4<T> rows[4];
        T m[4][4];
    };
};

} // namespace Internal

using Matrix4 = Internal::Matrix4<float>;
using Matrix4d = Internal::Matrix4<double>;

} // namespace SE