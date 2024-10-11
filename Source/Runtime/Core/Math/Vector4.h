/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Assertions.h>
#include <Core/CoreTypes.h>
#include <Core/Math/Math.h>

namespace SE
{
namespace Internal
{

template<typename T>
struct Vector4
{
public:
    NODISCARD ALWAYS_INLINE static Vector4 zero() { return Vector4(0, 0, 0, 0); }
    NODISCARD ALWAYS_INLINE static Vector4 one() { return Vector4(1, 1, 1, 1); }

    NODISCARD ALWAYS_INLINE static Vector4 unit_x() { return Vector4(1, 0, 0, 0); }
    NODISCARD ALWAYS_INLINE static Vector4 unit_y() { return Vector4(0, 1, 0, 0); }
    NODISCARD ALWAYS_INLINE static Vector4 unit_z() { return Vector4(0, 0, 1, 0); }
    NODISCARD ALWAYS_INLINE static Vector4 unit_w() { return Vector4(0, 0, 0, 1); }

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
    NODISCARD ALWAYS_INLINE T* value_ptr() { return &x; }
    NODISCARD ALWAYS_INLINE const T* value_ptr() const { return &x; }

    NODISCARD ALWAYS_INLINE T& operator[](Math::Axis axis)
    {
        const u8 value_index = static_cast<u8>(axis);
        SE_ASSERT(value_index < 4);
        return value_ptr()[value_index];
    }

    NODISCARD ALWAYS_INLINE const T& operator[](Math::Axis axis) const
    {
        const u8 value_index = static_cast<u8>(axis);
        SE_ASSERT(value_index < 4);
        return value_ptr()[value_index];
    }

public:
    T x;
    T y;
    T z;
    T w;
};

// Component-wise addition operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T> operator+(Vector4<T> a, Vector4<T> b)
{
    const Vector4<T> result = Vector4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T>& operator+=(Vector4<T>& self, Vector4<T> other)
{
    self.x += other.x;
    self.y += other.y;
    self.z += other.z;
    self.w += other.w;
    return self;
}

// Component-wise subtraction operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T> operator-(Vector4<T> lhs, Vector4<T> rhs)
{
    const Vector4<T> result = Vector4<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T>& operator-=(Vector4<T>& self, Vector4<T> other)
{
    self.x -= other.x;
    self.y -= other.y;
    self.z -= other.z;
    self.w -= other.w;
    return self;
}

// Component-wise negation operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T> operator-(Vector4<T> vector)
{
    const Vector4<T> result = Vector4(-vector.x, -vector.y, -vector.z, -vector.w);
    return result;
}

// Component-wise scalar multiplication operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T> operator*(Vector4<T> vector, T scalar)
{
    const Vector4<T> result = Vector4<T>(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar);
    return result;
}

// Component-wise scalar multiplication operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T> operator*(T scalar, Vector4<T> vector)
{
    const Vector4<T> result = Vector4<T>(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar);
    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T>& operator*=(Vector4<T>& self, T scalar)
{
    self.x *= scalar;
    self.y *= scalar;
    self.z *= scalar;
    self.w *= scalar;
    return self;
}

// Component-wise scalar division operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T> operator/(Vector4<T> vector, T scalar)
{
    const T inv_scalar = T(1) / scalar;
    const Vector4<T> result = Vector4<T>(vector.x * inv_scalar, vector.y * inv_scalar, vector.z * inv_scalar, vector.w * inv_scalar);
    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T>& operator/=(Vector4<T>& self, T scalar)
{
    const T inv_scalar = T(1) / scalar;
    self.x *= inv_scalar;
    self.y *= inv_scalar;
    self.z *= inv_scalar;
    self.w *= inv_scalar;
    return self;
}

} // namespace Internal

using Vector4 = Internal::Vector4<float>;
using Vector4d = Internal::Vector4<double>;

} // namespace SE
