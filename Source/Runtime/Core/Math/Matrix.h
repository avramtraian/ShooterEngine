/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Math/Vector.h>

namespace SE
{
namespace Math
{

#pragma region Matrix3

template<typename T>
struct Matrix3
{
public:
    ALWAYS_INLINE Matrix3()
        : rows { Vector3<T>(T(0)), Vector3<T>(T(0)), Vector3<T>(T(0)) }
    {}

    ALWAYS_INLINE Matrix3(const Matrix3& other)
        : rows { other.rows[0], other.rows[1], other.rows[2] }
    {}

    ALWAYS_INLINE Matrix3& operator=(const Matrix3& other)
    {
        rows[0] = other.rows[0];
        rows[1] = other.rows[1];
        rows[2] = other.rows[2];
        return *this;
    }

    ALWAYS_INLINE Matrix3(Vector3<T> row_0, Vector3<T> row_1, Vector3<T> row_2)
        : rows { row_0, row_1, row_2 }
    {}

public:
    union
    {
        T m[9];
        T v[3][3];
        Vector3<T> rows[3];
    };
};

#pragma endregion Matrix3

#pragma region Matrix4

template<typename T>
struct Matrix4
{
public:
    ALWAYS_INLINE Matrix4()
        : rows { Vector4<T>(T(0)), Vector4<T>(T(0)), Vector4<T>(T(0)), Vector4<T>(T(0)) }
    {}

    ALWAYS_INLINE Matrix4(const Matrix4& other)
        : rows { other.rows[0], other.rows[1], other.rows[2], other.rows[3] }
    {}

    ALWAYS_INLINE Matrix4& operator=(const Matrix4& other)
    {
        rows[0] = other.rows[0];
        rows[1] = other.rows[1];
        rows[2] = other.rows[2];
        rows[3] = other.rows[3];
        return *this;
    }

    ALWAYS_INLINE Matrix4(Vector4<T> row_0, Vector4<T> row_1, Vector4<T> row_2, Vector4<T> row_3)
        : rows { row_0, row_1, row_2, row_3 }
    {}

public:
    union
    {
        T m[16];
        T v[4][4];
        Vector4<T> rows[4];
    };
};

#pragma endregion Matrix4

} // namespace Math

using Matrix3 = Math::Matrix3<float>;
using Matrix4 = Math::Matrix4<float>;

using Matrix3d = Math::Matrix3<double>;
using Matrix4d = Math::Matrix4<double>;

} // namespace SE
