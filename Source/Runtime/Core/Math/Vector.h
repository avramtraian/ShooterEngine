/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Assertions.h>
#include <Core/Math/MathCore.h>

namespace SE
{
namespace Math
{

#pragma region Vector2

template<typename T>
struct Vector2
{
public:
    ALWAYS_INLINE constexpr Vector2()
        : x(T(0))
        , y(T(0))
    {}

    ALWAYS_INLINE constexpr Vector2(const Vector2& other)
        : x(other.x)
        , y(other.y)
    {}

    ALWAYS_INLINE constexpr Vector2& operator=(const Vector2& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    ALWAYS_INLINE constexpr Vector2(T in_x, T in_y)
        : x(in_x)
        , y(in_y)
    {}

    ALWAYS_INLINE constexpr explicit Vector2(T scalar)
        : x(scalar)
        , y(scalar)
    {}

public:
    NODISCARD ALWAYS_INLINE T* value_ptr() { return &x; }
    NODISCARD ALWAYS_INLINE const T* value_ptr() const { return &x; }

    NODISCARD ALWAYS_INLINE T& operator[](u8 axis)
    {
        SE_ASSERT(axis < 2);
        return value_ptr()[axis];
    }

    NODISCARD ALWAYS_INLINE const T& operator[](u8 axis) const
    {
        SE_ASSERT(axis < 2);
        return value_ptr()[axis];
    }

    NODISCARD ALWAYS_INLINE T& operator[](VectorAxis axis) { return operator[](static_cast<u8>(axis)); }
    NODISCARD ALWAYS_INLINE const T& operator[](VectorAxis axis) const { return operator[](static_cast<u8>(axis)); }

public:
    T x;
    T y;
};

template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T> operator+(Vector2<T> lhs, Vector2<T> rhs)
{
    const Vector2<T> result = Vector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
    return result;
}

#pragma endregion Vector2

#pragma region Vector3

template<typename T>
struct Vector3
{
public:
    ALWAYS_INLINE constexpr Vector3()
        : x(T(0))
        , y(T(0))
        , z(T(0))
    {}

    ALWAYS_INLINE constexpr Vector3(const Vector3& other)
        : x(other.x)
        , y(other.y)
        , z(other.z)
    {}

    ALWAYS_INLINE constexpr Vector3& operator=(const Vector3& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    ALWAYS_INLINE constexpr Vector3(T in_x, T in_y, T in_z)
        : x(in_x)
        , y(in_y)
        , z(in_z)
    {}

    ALWAYS_INLINE constexpr explicit Vector3(T scalar)
        : x(scalar)
        , y(scalar)
        , z(scalar)
    {}

public:
    NODISCARD ALWAYS_INLINE T* value_ptr() { return &x; }
    NODISCARD ALWAYS_INLINE const T* value_ptr() const { return &x; }

    NODISCARD ALWAYS_INLINE T& operator[](u8 axis)
    {
        SE_ASSERT(axis < 3);
        return value_ptr()[axis];
    }

    NODISCARD ALWAYS_INLINE const T& operator[](u8 axis) const
    {
        SE_ASSERT(axis < 3);
        return value_ptr()[axis];
    }

    NODISCARD ALWAYS_INLINE T& operator[](VectorAxis axis) { return operator[](static_cast<u8>(axis)); }
    NODISCARD ALWAYS_INLINE const T& operator[](VectorAxis axis) const { return operator[](static_cast<u8>(axis)); }

public:
    T x;
    T y;
    T z;
};

template<typename T>
NODISCARD ALWAYS_INLINE Vector3<T> operator+(Vector3<T> lhs, Vector3<T> rhs)
{
    const Vector3<T> result = Vector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    return result;
}

#pragma endregion Vector3

#pragma region Vector4

template<typename T>
struct Vector4
{
public:
    ALWAYS_INLINE constexpr Vector4()
        : x(T(0))
        , y(T(0))
        , z(T(0))
        , w(T(0))
    {}

    ALWAYS_INLINE constexpr Vector4(const Vector4& other)
        : x(other.x)
        , y(other.y)
        , z(other.z)
        , w(other.w)
    {}

    ALWAYS_INLINE constexpr Vector4& operator=(const Vector4& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    ALWAYS_INLINE constexpr Vector4(T in_x, T in_y, T in_z, T in_w)
        : x(in_x)
        , y(in_y)
        , z(in_z)
        , w(in_w)
    {}

    ALWAYS_INLINE constexpr explicit Vector4(T scalar)
        : x(scalar)
        , y(scalar)
        , z(scalar)
        , w(scalar)
    {}

public:
    NODISCARD ALWAYS_INLINE T* value_ptr() { return &x; }
    NODISCARD ALWAYS_INLINE const T* value_ptr() const { return &x; }

    NODISCARD ALWAYS_INLINE T& operator[](u8 axis)
    {
        SE_ASSERT(axis < 4);
        return value_ptr()[axis];
    }

    NODISCARD ALWAYS_INLINE const T& operator[](u8 axis) const
    {
        SE_ASSERT(axis < 4);
        return value_ptr()[axis];
    }

    NODISCARD ALWAYS_INLINE T& operator[](VectorAxis axis) { return operator[](static_cast<u8>(axis)); }
    NODISCARD ALWAYS_INLINE const T& operator[](VectorAxis axis) const { return operator[](static_cast<u8>(axis)); }

public:
    T x;
    T y;
    T z;
    T w;
};

template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T> operator+(Vector4<T> lhs, Vector4<T> rhs)
{
    const Vector4<T> result = Vector4<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
    return result;
}

#pragma endregion Vector4

} // namespace Math

using Vector2 = Math::Vector2<float>;
using Vector3 = Math::Vector3<float>;
using Vector4 = Math::Vector4<float>;

using Vector2d = Math::Vector2<double>;
using Vector3d = Math::Vector3<double>;
using Vector4d = Math::Vector4<double>;

} // namespace SE
