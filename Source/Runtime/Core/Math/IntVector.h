/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>

namespace SE
{
namespace Math
{

#pragma region IntVector2

template<typename T>
struct IntVector2
{
public:
    ALWAYS_INLINE constexpr IntVector2()
        : x(T(0))
        , y(T(0))
    {}

    ALWAYS_INLINE constexpr IntVector2(const IntVector2& other)
        : x(other.x)
        , y(other.y)
    {}

    ALWAYS_INLINE constexpr IntVector2& operator=(const IntVector2& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    ALWAYS_INLINE constexpr IntVector2(T in_x, T in_y)
        : x(in_x)
        , y(in_y)
    {}

    ALWAYS_INLINE constexpr IntVector2(T scalar)
        : x(scalar)
        , y(scalar)
    {}

public:
    T x;
    T y;
};

template<typename T>
NODISCARD ALWAYS_INLINE IntVector2<T> operator-(IntVector2<T> lhs, IntVector2<T> rhs)
{
    IntVector2<T> result = IntVector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
    return result;
}

#pragma endregion IntVector2

} // namespace Math

using IntVector2 = Math::IntVector2<i32>;

using Int8Vector2 = Math::IntVector2<i8>;

using Int16Vector2 = Math::IntVector2<i16>;

using Int64Vector2 = Math::IntVector2<i64>;

using UIntVector2 = Math::IntVector2<u32>;

using UInt8Vector2 = Math::IntVector2<u8>;

using UInt16Vector2 = Math::IntVector2<u16>;

using UInt64Vector2 = Math::IntVector2<u64>;

} // namespace SE
