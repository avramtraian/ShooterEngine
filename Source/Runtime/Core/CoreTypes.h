/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreDefines.h"

namespace SE
{

#if SE_PLATFORM_WINDOWS
    using uint8         = unsigned char;
    using uint16        = unsigned short;
    using uint32        = unsigned int;
    using uint64        = unsigned long long;

    using int8          = signed char;
    using int16         = signed short;
    using int32         = signed int;
    using int64         = signed long long;

    using usize         = unsigned long long;
    using ssize         = signed long long;
    using uintptr       = unsigned long long;

    using float32       = float;
    using float64       = double;
#endif // Platform switch.

using ReadonlyByte      = const uint8;
using WriteonlyByte     = uint8;
using ReadWriteByte     = uint8;

using ReadonlyBytes     = ReadonlyByte*;
using WriteonlyBytes    = WriteonlyByte*;
using ReadWriteBytes    = ReadWriteByte*;

namespace Internal
{

template<typename T> struct RemoveReference         { using Type = T; };
template<typename T> struct RemoveReference<T&>     { using Type = T; };
template<typename T> struct RemoveReference<T&&>    { using Type = T; };

template<typename T> struct RemoveConst             { using Type = T; };
template<typename T> struct RemoveConst<const T>    { using Type = T; };

} // namespace Internal

template<typename T> using RemoveReference  = typename Internal::RemoveReference<T>::Type;
template<typename T> using RemoveConst      = typename Internal::RemoveConst<T>::Type;

template<typename T>
NODISCARD ALWAYS_INLINE constexpr RemoveReference<T>&& move(T&& value)
{
    // https://en.cppreference.com/w/cpp/utility/move
    return static_cast<RemoveReference<T>&&>(value);
}

template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>& value)
{
    // https://en.cppreference.com/w/cpp/utility/forward
    return static_cast<T&&>(value);
}

template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>&& value)
{
    // https://en.cppreference.com/w/cpp/utility/forward
    return static_cast<T&&>(value);
}

#define SE_MAKE_NONCOPYABLE(type_name)          \
    type_name(const type_name&) = delete;       \
    type_name& operator=(const type_name&) = delete;

#define SE_MAKE_NONMOVABLE(type_name)           \
    type_name(type_name&&) noexcept = delete;   \
    type_name& operator=(type_name&&) noexcept = delete;

//
// Ensure that the fixed primitive types are matching the expected size.
//

static_assert(sizeof(uint8) == 1);
static_assert(sizeof(uint16) == 2);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(uint64) == 8);

static_assert(sizeof(int8) == 1);
static_assert(sizeof(int16) == 2);
static_assert(sizeof(int32) == 4);
static_assert(sizeof(int64) == 8);

static_assert(sizeof(usize) == sizeof(void*));
static_assert(sizeof(ssize) == sizeof(void*));
static_assert(sizeof(uintptr) == sizeof(void*));

static_assert(sizeof(float32) == 4);
static_assert(sizeof(float64) == 8);

} // namespace SE