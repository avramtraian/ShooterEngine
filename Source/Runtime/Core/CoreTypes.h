/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreDefines.h>
#include <type_traits>

#if SE_PLATFORM_WINDOWS
namespace SE
{

//
// Fixed-size primitive types that represent an unsigned integer.
// Their sizes can always be assumed as they are guaranteed to be the same on all platforms.
//
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

//
// Fixed-size primitive types that represent an signed integer.
// Their sizes can always be assumed as they are guaranteed to be the same on all platforms.
//
using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

//
// Primitive types that represent integers which hold a size or memory address.
// Never assume their sizes, as they are not guaranteed to be the same on all platforms.
//
using usize = u64;
using ssize = i64;
using uintptr = u64;
using intptr = i64;

} // namespace SE
#endif // SE_PLATFORM_WINDOWS

namespace SE
{

using ReadonlyByte = const u8;
using WriteonlyByte = u8;
using ReadWriteByte = u8;

using ReadonlyBytes = ReadonlyByte*;
using WriteonlyBytes = WriteonlyByte*;
using ReadWriteBytes = ReadWriteByte*;

using UnicodeCodepoint = u32;

constexpr usize invalid_size = static_cast<usize>(-1);
constexpr UnicodeCodepoint invalid_unicode_codepoint = static_cast<UnicodeCodepoint>(-1);

} // namespace SE

// Marks the type in which this macro is placed as non-copyable, by marking the
// copy constructor and assignment operator as deleted.
#define SE_MAKE_NONCOPYABLE(type_name)    \
    type_name(const type_name&) = delete; \
    type_name& operator=(const type_name&) = delete

// Marks the type in which this macro is placed as non-movable, by marking the
// move constructor and assignment operator as deleted.
#define SE_MAKE_NONMOVABLE(type_name)         \
    type_name(type_name&&) noexcept = delete; \
    type_name& operator=(type_name&&) noexcept = delete

namespace SE
{

// clang-format off
namespace Detail
{

template<typename T> struct RemoveReference         { using Type = T; };
template<typename T> struct RemoveReference<T&>     { using Type = T; };
template<typename T> struct RemoveReference<T&&>    { using Type = T; };

template<typename T> struct RemoveConst             { using Type = T; };
template<typename T> struct RemoveConst<const T>    { using Type = T; };

template<typename T> struct RemovePointer           { using Type = T; };
template<typename T> struct RemovePointer<T*>       { using Type = T; };

template<typename T>    struct IsIntegral      { static constexpr bool value = false;  static constexpr bool is_signed = false; };
template<>              struct IsIntegral<u8>  { static constexpr bool value = true;   static constexpr bool is_signed = false; };
template<>              struct IsIntegral<u16> { static constexpr bool value = true;   static constexpr bool is_signed = false; };
template<>              struct IsIntegral<u32> { static constexpr bool value = true;   static constexpr bool is_signed = false; };
template<>              struct IsIntegral<u64> { static constexpr bool value = true;   static constexpr bool is_signed = false; };
template<>              struct IsIntegral<i8>  { static constexpr bool value = true;   static constexpr bool is_signed = true; };
template<>              struct IsIntegral<i16> { static constexpr bool value = true;   static constexpr bool is_signed = true; };
template<>              struct IsIntegral<i32> { static constexpr bool value = true;   static constexpr bool is_signed = true; };
template<>              struct IsIntegral<i64> { static constexpr bool value = true;   static constexpr bool is_signed = true; };

} // namespace Detail
// clang-format on

//
// Remove the reference from a type.
// For example, `const int&` becomes `const int`, while `float` remains `float`.
//
template<typename T>
using RemoveReference = typename Detail::RemoveReference<T>::Type;

//
// Remove the constness from a type.
// For example, `const int&` becomes `int&`, while `float` remains `float`.
//
template<typename T>
using RemoveConst = typename Detail::RemoveConst<T>::Type;

template<typename T>
constexpr bool is_integral = Detail::IsIntegral<T>::value;
template<typename T>
constexpr bool is_signed_integral = is_integral<T> && Detail::IsIntegral<T>::is_signed;
template<typename T>
constexpr bool is_unsigned_integral = is_integral<T> && !Detail::IsIntegral<T>::is_signed;

// Wrapper around `std::is_base_of_v`.
template<typename DerivedType, typename BaseType>
constexpr bool is_derived_from = std::is_base_of_v<BaseType, DerivedType>;

// Wrapper around `std::is_base_of_v`.
template<typename BaseType, typename DerivedType>
constexpr bool is_base_of = std::is_base_of_v<BaseType, DerivedType>;

//
// Remove the pointer from a type.
// For example, `const int*` becomes `const int`, while `float` remains `float`.
//
template<typename T>
using RemovePointer = typename Detail::RemovePointer<T>::Type;

//
// Used in move semantics. Follows the C++ standard signature, which can be found at:
// https://en.cppreference.com/w/cpp/utility/move
//
template<typename T>
NODISCARD ALWAYS_INLINE constexpr RemoveReference<T>&& move(T&& value)
{
    return static_cast<RemoveReference<T>&&>(value);
}

//
// Used in move semantics. Follows the C++ standard signature, which can be found at:
// https://en.cppreference.com/w/cpp/utility/forward
//
template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>& value)
{
    return static_cast<T&&>(value);
}

//
// Used in move semantics. Follows the C++ standard signature, which can be found at:
// https://en.cppreference.com/w/cpp/utility/forward
//
template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>&& value)
{
    return static_cast<T&&>(value);
}

} // namespace SE
