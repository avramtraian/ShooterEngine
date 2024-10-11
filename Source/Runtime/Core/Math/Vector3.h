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
struct Vector3
{
public:
    NODISCARD ALWAYS_INLINE static Vector3 zero() { return Vector3(0, 0, 0); }
    NODISCARD ALWAYS_INLINE static Vector3 one() { return Vector3(1, 1, 1); }

    NODISCARD ALWAYS_INLINE static Vector3 unit_x() { return Vector3(1, 0, 0); }
    NODISCARD ALWAYS_INLINE static Vector3 unit_y() { return Vector3(0, 1, 0); }
    NODISCARD ALWAYS_INLINE static Vector3 unit_z() { return Vector3(0, 0, 1); }

public:
    NODISCARD ALWAYS_INLINE static T length_squared(Vector3 vector)
    {
        const T result = (vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z);
        return result;
    }

    NODISCARD ALWAYS_INLINE static T length(Vector3 vector)
    {
        const T length_squared = Vector3::length_squared(vector);
        const T result = Math::sqrt(length_squared);
        return result;
    }

    NODISCARD ALWAYS_INLINE static T dot(Vector3 a, Vector3 b)
    {
        const T result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
        return result;
    }

    NODISCARD ALWAYS_INLINE static Vector3 cross(Vector3 lhs, Vector3 rhs)
    {
        // clang-format off
        const Vector3 result = Vector3(
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x
        );
        // clang-format on
        return result;
    }

    NODISCARD ALWAYS_INLINE static Vector3 normalize(Vector3 vector)
    {
        const T length = Vector3::length(vector);
        SE_ASSERT(length > Math::small_number);
        const T inv_length = T(1) / length;

        const Vector3 result = Vector3(vector.x * inv_length, vector.y * inv_length, vector.z * inv_length);
        return result;
    }

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
    // Wrapper around `Vector3::length_squared`.
    // See the above function declaration for documentation.
    NODISCARD ALWAYS_INLINE T length_squared() const { return Vector3::length_squared(*this); }

    // Wrapper around `Vector3::length`.
    // See the above function declaration for documentation.
    NODISCARD ALWAYS_INLINE T length() const { return Vector3::length(*this); }

    // Wrapper around `Vector3::normalized`.
    // See the above function declaration for documentation.
    NODISCARD ALWAYS_INLINE Vector3 normalized() const { return Vector3::normalize(*this); }

public:
    NODISCARD ALWAYS_INLINE T* value_ptr() { return &x; }
    NODISCARD ALWAYS_INLINE const T* value_ptr() const { return &x; }

    NODISCARD ALWAYS_INLINE T& operator[](Math::Axis axis)
    {
        const u8 value_index = static_cast<u8>(axis);
        SE_ASSERT(value_index < 3);
        return value_ptr()[value_index];
    }

    NODISCARD ALWAYS_INLINE const T& operator[](Math::Axis axis) const
    {
        const u8 value_index = static_cast<u8>(axis);
        SE_ASSERT(value_index < 3);
        return value_ptr()[value_index];
    }

public:
    T x;
    T y;
    T z;
};

// Component-wise addition operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector3<T> operator+(Vector3<T> a, Vector3<T> b)
{
    const Vector3<T> result = Vector3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
    return result;
}

template<typename T>
ALWAYS_INLINE Vector3<T>& operator+=(Vector3<T>& self, Vector3<T> other)
{
    self.x += other.x;
    self.y += other.y;
    self.z += other.z;
    return self;
}

// Component-wise subtraction operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector3<T> operator-(Vector3<T> lhs, Vector3<T> rhs)
{
    const Vector3<T> result = Vector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    return result;
}

template<typename T>
ALWAYS_INLINE Vector3<T>& operator-=(Vector3<T>& self, Vector3<T> other)
{
    self.x -= other.x;
    self.y -= other.y;
    self.z -= other.z;
    return self;
}

// Component-wise negation operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector3<T> operator-(Vector3<T> vector)
{
    const Vector3<T> result = Vector3(-vector.x, -vector.y, -vector.z);
    return result;
}

// Component-wise scalar multiplication operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector3<T> operator*(Vector3<T> vector, T scalar)
{
    const Vector3<T> result = Vector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
    return result;
}

// Component-wise scalar multiplication operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector3<T> operator*(T scalar, Vector3<T> vector)
{
    const Vector3<T> result = Vector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
    return result;
}

template<typename T>
ALWAYS_INLINE Vector3<T>& operator*=(Vector3<T>& self, T scalar)
{
    self.x *= scalar;
    self.y *= scalar;
    self.z *= scalar;
    return self;
}

// Component-wise scalar division operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector3<T> operator/(Vector3<T> vector, T scalar)
{
    const T inv_scalar = T(1) / scalar;
    const Vector3<T> result = Vector3(vector.x * inv_scalar, vector.y * inv_scalar, vector.z * inv_scalar);
    return result;
}

template<typename T>
ALWAYS_INLINE Vector3<T>& operator/=(Vector3<T>& self, T scalar)
{
    const T inv_scalar = T(1) / scalar;
    self.x *= inv_scalar;
    self.y *= inv_scalar;
    self.z *= inv_scalar;
    return self;
}

} // namespace Internal

using Vector3 = Internal::Vector3<float>;
using Vector3d = Internal::Vector3<double>;

} // namespace SE
