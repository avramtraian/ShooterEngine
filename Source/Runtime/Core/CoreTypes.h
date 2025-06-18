// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreDefines.h>

#include <initializer_list>
#include <new>
#include <type_traits>

namespace SE
{

using uint8  = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using int8  = signed char;
using int16 = signed short;
using int32 = signed int;
using int64 = signed long long;

using usize   = unsigned long long;
using ssize   = signed long long;
using uintptr = unsigned long long;

using ReadonlyByte  = const uint8;
using WriteonlyByte = uint8;
using ReadWriteByte = uint8;

using ReadonlyBytes  = ReadonlyByte*;
using WriteonlyBytes = WriteonlyByte*;
using ReadWriteBytes = ReadWriteByte*;

using NullptrType = decltype(nullptr);

static constexpr usize INVALID_SIZE   = static_cast<usize>(-1);
static constexpr usize INVALID_OFFSET = static_cast<usize>(-1);
static constexpr usize INVALID_INDEX  = static_cast<usize>(-1);

namespace Implementation
{

template<typename T> struct RemoveReference      { using Type = T; };
template<typename T> struct RemoveReference<T&>  { using Type = T; };
template<typename T> struct RemoveReference<T&&> { using Type = T; };

template<typename T> struct RemoveConst          { using Type = T; };
template<typename T> struct RemoveConst<const T> { using Type = T; };

template<typename T> struct RemovePointer        { using Type = T; };
template<typename T> struct RemovePointer<T*>    { using Type = T; };

}

template<typename T> using RemoveReference = typename Implementation::RemoveReference<T>::Type;
template<typename T> using RemoveConst     = typename Implementation::RemoveConst<T>::Type;
template<typename T> using RemovePointer   = typename Implementation::RemovePointer<T>::Type;

template<typename T>
NODISCARD FORCEINLINE RemoveReference<T>&& Move(T&& instance) noexcept
{
    return static_cast<RemoveReference<T>&&>(instance);
}

template<typename T>
NODISCARD FORCEINLINE T&& Forward(RemoveReference<T>& instance) noexcept
{
    return static_cast<T&&>(instance);
}

template<typename T>
NODISCARD FORCEINLINE T&& Forward(RemoveReference<T>&& instance) noexcept
{
    return static_cast<T&&>(instance);
}

}

/* Marks the copy constructor and the copy assignmnet operator as deleted. */
#define SE_MAKE_NONCOPYABLE(type_name)    \
    type_name(const type_name&) = delete; \
    type_name& operator=(const type_name&) = delete

/* Marks the move constructor and the move assignmnet operator as deleted. */
#define SE_MAKE_NONMOVABLE(type_name)         \
    type_name(type_name&&) noexcept = delete; \
    type_name& operator=(type_name&&) noexcept = delete

/* Marks the type as non-copyable and non-movable using the above macros and
 * marks the default constructor and destructor as deleted, so no instance of this type can be created. */
#define SE_MAKE_SINGLETON(type_name)    \
    private:                            \
        SE_MAKE_NONCOPYABLE(type_name); \
        SE_MAKE_NONMOVABLE(type_name);  \
        type_name() = delete;           \
        ~type_name() = delete;
