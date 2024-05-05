/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreDefines.h"

namespace SE {

#if SE_PLATFORM_WINDOWS

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using usize = unsigned long long;
using ssize = signed long long;
using uintptr = unsigned long long;

using f32 = float;
using f64 = double;

#endif // Platform switch.

using ReadonlyByte = const u8;
using WriteonlyByte = u8;
using ReadWriteByte = u8;

using ReadonlyBytes = ReadonlyByte*;
using WriteonlyBytes = WriteonlyByte*;
using ReadWriteBytes = ReadWriteByte*;

using UnicodeCodepoint = u32;

constexpr usize invalid_size = static_cast<usize>(-1);
constexpr UnicodeCodepoint invalid_unicode_codepoint = static_cast<UnicodeCodepoint>(-1);

namespace Internal {

template<typename T>
struct RemoveReference {
    using Type = T;
};
template<typename T>
struct RemoveReference<T&> {
    using Type = T;
};
template<typename T>
struct RemoveReference<T&&> {
    using Type = T;
};

template<typename T>
struct RemoveConst {
    using Type = T;
};
template<typename T>
struct RemoveConst<const T> {
    using Type = T;
};

template<typename T>
struct IsIntegral {
    static constexpr bool value = false;
    static constexpr bool is_signed = false;
};

template<>
struct IsIntegral<u8> {
    static constexpr bool value = true;
    static constexpr bool is_signed = false;
};
template<>
struct IsIntegral<u16> {
    static constexpr bool value = true;
    static constexpr bool is_signed = false;
};
template<>
struct IsIntegral<u32> {
    static constexpr bool value = true;
    static constexpr bool is_signed = false;
};
template<>
struct IsIntegral<u64> {
    static constexpr bool value = true;
    static constexpr bool is_signed = false;
};
template<>
struct IsIntegral<i8> {
    static constexpr bool value = true;
    static constexpr bool is_signed = true;
};
template<>
struct IsIntegral<i16> {
    static constexpr bool value = true;
    static constexpr bool is_signed = true;
};
template<>
struct IsIntegral<i32> {
    static constexpr bool value = true;
    static constexpr bool is_signed = true;
};
template<>
struct IsIntegral<i64> {
    static constexpr bool value = true;
    static constexpr bool is_signed = true;
};

} // namespace Internal

template<typename T>
using RemoveReference = typename Internal::RemoveReference<T>::Type;
template<typename T>
using RemoveConst = typename Internal::RemoveConst<T>::Type;

template<typename T>
constexpr bool is_integral = Internal::IsIntegral<T>::value;
template<typename T>
constexpr bool is_signed_integral = is_integral<T> && Internal::IsIntegral<T>::is_signed;
template<typename T>
constexpr bool is_unsigned_integral = is_integral<T> && !Internal::IsIntegral<T>::is_signed;

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

#define SE_MAKE_NONCOPYABLE(type_name)    \
    type_name(const type_name&) = delete; \
    type_name& operator=(const type_name&) = delete;

#define SE_MAKE_NONMOVABLE(type_name)         \
    type_name(type_name&&) noexcept = delete; \
    type_name& operator=(type_name&&) noexcept = delete;

//
// Ensure that the fixed primitive types are matching the expected size.
//

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);

static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);

static_assert(sizeof(usize) == sizeof(void*));
static_assert(sizeof(ssize) == sizeof(void*));
static_assert(sizeof(uintptr) == sizeof(void*));

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

} // namespace SE