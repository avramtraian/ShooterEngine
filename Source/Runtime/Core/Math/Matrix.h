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
    NODISCARD ALWAYS_INLINE static constexpr Matrix3 identity()
    {
        // clang-format off
        return Matrix3(
            Vector3<T>(T(1), T(0), T(0)),
            Vector3<T>(T(0), T(1), T(0)),
            Vector3<T>(T(0), T(0), T(1))
        );
        // clang-format on
    }

public:
    ALWAYS_INLINE constexpr Matrix3()
        : rows { Vector3<T>(T(0)), Vector3<T>(T(0)), Vector3<T>(T(0)) }
    {}

    ALWAYS_INLINE constexpr Matrix3(const Matrix3& other)
        : rows { other.rows[0], other.rows[1], other.rows[2] }
    {}

    ALWAYS_INLINE constexpr Matrix3& operator=(const Matrix3& other)
    {
        rows[0] = other.rows[0];
        rows[1] = other.rows[1];
        rows[2] = other.rows[2];
        return *this;
    }

    ALWAYS_INLINE constexpr Matrix3(Vector3<T> row_0, Vector3<T> row_1, Vector3<T> row_2)
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
    NODISCARD ALWAYS_INLINE static constexpr Matrix4 identity()
    {
        // clang-format off
        return Matrix4(
            Vector4<T>(T(1), T(0), T(0), T(0)),
            Vector4<T>(T(0), T(1), T(0), T(0)),
            Vector4<T>(T(0), T(0), T(1), T(0)),
            Vector4<T>(T(0), T(0), T(0), T(1))
        );
        // clang-format on
    }

public:
    ALWAYS_INLINE constexpr Matrix4()
        : rows { Vector4<T>(T(0)), Vector4<T>(T(0)), Vector4<T>(T(0)), Vector4<T>(T(0)) }
    {}

    ALWAYS_INLINE constexpr Matrix4(const Matrix4& other)
        : rows { other.rows[0], other.rows[1], other.rows[2], other.rows[3] }
    {}

    ALWAYS_INLINE constexpr Matrix4& operator=(const Matrix4& other)
    {
        rows[0] = other.rows[0];
        rows[1] = other.rows[1];
        rows[2] = other.rows[2];
        rows[3] = other.rows[3];
        return *this;
    }

    ALWAYS_INLINE constexpr Matrix4(Vector4<T> row_0, Vector4<T> row_1, Vector4<T> row_2, Vector4<T> row_3)
        : rows { row_0, row_1, row_2, row_3 }
    {}

public:
    NODISCARD ALWAYS_INLINE static Matrix4 transpose(const Matrix4& matrix);
    
    NODISCARD ALWAYS_INLINE static Matrix4 inverse(const Matrix4& matrix);

    NODISCARD ALWAYS_INLINE static Matrix4 translate(Vector3<T> translation);

    NODISCARD ALWAYS_INLINE static Matrix4 rotate(Vector3<T> rotation);

    NODISCARD ALWAYS_INLINE static Matrix4 scale(Vector3<T> scale);

    //
    // Creates a perspective projection matrix from the given parameters.
    // The vertical FOV angle must be specified in radians.
    //
    NODISCARD ALWAYS_INLINE static Matrix4 perspective(T vertical_fov, T aspect_ratio, T clip_near, T clip_far);

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
