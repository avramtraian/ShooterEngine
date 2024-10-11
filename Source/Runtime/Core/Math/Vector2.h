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
struct Vector2
{
public:
    NODISCARD ALWAYS_INLINE static Vector2 zero() { return Vector2(T(0), T(0)); }

    NODISCARD ALWAYS_INLINE static Vector2 unit_x() { return Vector2(T(1), T(0)); }
    NODISCARD ALWAYS_INLINE static Vector2 unit_y() { return Vector2(T(0), T(1)); }

public:
    NODISCARD ALWAYS_INLINE static T length_squared(Vector2 vector)
    {
        const T result = (vector.x * vector.x) + (vector.y * vector.y);
        return result;
    }

    NODISCARD ALWAYS_INLINE static T length(Vector2 vector)
    {
        const T length_squared = Vector2::length_squared(vector);
        const T result = Math::sqrt(length_squared);
        return result;
    }

    NODISCARD ALWAYS_INLINE static T dot(Vector2 a, Vector2 b)
    {
        const T result = (a.x * b.x) + (a.y * b.y);
        return result;
    }

    NODISCARD ALWAYS_INLINE static Vector2 normalize(Vector2 vector)
    {
        const T length = Vector2::length(vector);
        SE_ASSERT(length > Math::small_number);
        const T inv_length = T(1) / length;
        const Vector2 result = Vector2(vector.x * inv_length, vector.y * inv_length);
        return result;
    }

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
    // Wrapper around `Vector2::length_squared`.
    // See the above function declaration for documentation.
    NODISCARD ALWAYS_INLINE T length_squared() const { return Vector2::length_squared(*this); }

    // Wrapper around `Vector2::length`.
    // See the above function declaration for documentation.
    NODISCARD ALWAYS_INLINE T length() const { return Vector2::length(*this); }

    // Wrapper around `Vector2::normalized`.
    // See the above function declaration for documentation.
    NODISCARD ALWAYS_INLINE Vector2 normalized() const { return Vector2::normalize(*this); }

public:
    NODISCARD ALWAYS_INLINE T* value_ptr() { return &x; }
    NODISCARD ALWAYS_INLINE const T* value_ptr() const { return &x; }

    NODISCARD ALWAYS_INLINE T& operator[](Math::Axis axis)
    {
        const u8 value_index = static_cast<u8>(axis);
        SE_ASSERT(value_index < 2);
        return value_ptr()[value_index];
    }

    NODISCARD ALWAYS_INLINE const T& operator[](Math::Axis axis) const
    {
        const u8 value_index = static_cast<u8>(axis);
        SE_ASSERT(value_index < 2);
        return value_ptr()[value_index];
    }

public:
    T x;
    T y;
};

// Component-wise addition operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T> operator+(Vector2<T> a, Vector2<T> b)
{
    const Vector2<T> result = Vector2<T>(a.x + b.x, a.y + b.y);
    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T>& operator+=(Vector2<T>& self, Vector2<T> other)
{
    self.x += other.x;
    self.y += other.y;
    return self;
}

// Component-wise subtraction operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T> operator-(Vector2<T> lhs, Vector2<T> rhs)
{
    const Vector2<T> result = Vector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T>& operator-=(Vector2<T>& self, Vector2<T> other)
{
    self.x -= other.x;
    self.y -= other.y;
    return self;
}

// Component-wise negation operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T> operator-(Vector2<T> vector)
{
    const Vector2<T> result = Vector2<T>(-vector.x, -vector.y);
    return result;
}

// Component-wise scalar multiplication operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T> operator*(Vector2<T> vector, T scalar)
{
    const Vector2<T> result = Vector2<T>(vector.x * scalar, vector.y * scalar);
    return result;
}

// Component-wise scalar multiplication operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T> operator*(T scalar, Vector2<T> vector)
{
    const Vector2<T> result = Vector2<T>(vector.x * scalar, vector.y * scalar);
    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T>& operator*=(Vector2<T>& self, T scalar)
{
    self.x *= scalar;
    self.y *= scalar;
    return self;
}

// Component-wise scalar division operator.
template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T> operator/(Vector2<T> vector, T scalar)
{
    const T inv_scalar = T(1) / scalar;
    const Vector2<T> result = Vector2<T>(vector.x * inv_scalar, vector.y * inv_scalar);
    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector2<T>& operator/=(Vector2<T>& self, T scalar)
{
    const T inv_scalar = T(1) / scalar;
    self.x *= inv_scalar;
    self.y *= inv_scalar;
    return self;
}

} // namespace Internal

using Vector2 = Internal::Vector2<float>;
using Vector2d = Internal::Vector2<double>;

} // namespace SE
