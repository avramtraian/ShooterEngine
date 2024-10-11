/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

//======================================================================================
// PLATFORM CONFIGURATION MACROS.
//======================================================================================
//
// NOTE: The platform and configurations macros are set by the build system.
// It is the responsability of the build system to ensure that they are set correctly.
//

#ifndef SE_PLATFORM_WINDOWS
    #define SE_PLATFORM_WINDOWS 0
#endif // SE_PLATFORM_WINDOWS

//
// Ensure that at least one platform macro is set to 1.
// Otherwise, the project configuration is wrong and a compiler error should be raised.
//
#if !SE_PLATFORM_WINDOWS
    #error Unknown or unsupported platform!
#endif // Any supported platform.

//======================================================================================
// BUILD CONFIGURATION MACROS.
//======================================================================================

#ifndef SE_CONFIGURATION_DEBUG
    #define SE_CONFIGURATION_DEBUG 0
#endif // SE_CONFIGURATION_DEBUG

#ifndef SE_CONFIGURATION_DEVELOPMENT
    #define SE_CONFIGURATION_DEVELOPMENT 0
#endif // SE_CONFIGURATION_DEVELOPMENT

#ifndef SE_CONFIGURATION_SHIPPING
    #define SE_CONFIGURATION_SHIPPING 0
#endif // SE_CONFIGURATION_SHIPPING

//
// Ensure that at least one build configuration macro is set to 1.
// Otherwise, the project configuration is wrong and a compiler error should be raised.
//
#if !SE_CONFIGURATION_DEBUG && !SE_CONFIGURATION_DEVELOPMENT && !SE_CONFIGURATION_SHIPPING
    #error Unknown or unsupported build configuration!
#endif // Any supported build configuration.

#ifndef SE_CONFIGURATION_TARGET_EDITOR
    #define SE_CONFIGURATION_TARGET_EDITOR 0
#endif // SE_CONFIGURATION_TARGET_EDITOR

#ifndef SE_CONFIGURATION_TARGET_GAME
    #define SE_CONFIGURATION_TARGET_GAME 0
#endif // SE_CONFIGURATION_TARGET_GAME

#if !SE_CONFIGURATION_TARGET_EDITOR && !SE_CONFIGURATION_TARGET_GAME
    #error Unknown or unsupported build configuration target!
#endif // Any supported build configuration target.

//======================================================================================
// COMPILER CONFIGURATION MACROS.
//======================================================================================

// The `__clang__` is only set when compiling using clang.
#ifdef __clang__
    #define SE_COMPILER_CLANG 1
#else
    // The `_MSC_BUILD` is defined when compiling with MSVC, or sometimes when using clang. As we
    // already checked for the clang compiler, we can be confident that this flag is only set when
    // using MSVC.
    #ifdef _MSC_BUILD
        #define SE_COMPILER_MSVC 1
    #endif // _MSC_BUILD

    // Similarly to the `_MSC_BUILD` flag, `__gnuc__` is only set when compiling using GCC.
    #ifdef __GNUC__
        #define SE_COMPILER_GCC 1
    #endif // __GNUC__
#endif // __clang__

#ifndef SE_COMPILER_MSVC
    #define SE_COMPILER_MSVC 0
#endif // SE_COMPILER_MSVC

#ifndef SE_COMPILER_CLANG
    #define SE_COMPILER_CLANG 0
#endif // SE_COMPILER_CLANG

#ifndef SE_COMPILER_GCC
    #define SE_COMPILER_GCC 0
#endif // SE_COMPILER_GCC

//
// Ensure that at least one compiler macro is set to 1.
// Otherwise, an unknown or unsupported compiler is used, which should raise a compiler error.
//
#if !SE_COMPILER_MSVC && !SE_COMPILER_CLANG && !SE_COMPILER_GCC
    #error Unknown or unsupported compiler!
#endif // Any supported compiler.

//======================================================================================
// UTILITY (GENERAL PURPOSE) MACROS.
//======================================================================================

#if SE_COMPILER_MSVC
    // Hint for the compiler that the function should always be inlined.
    #define ALWAYS_INLINE __forceinline

    // Traps the debugger. Triggers a breakpoint if a debugger is attached or crashes the program otherwise.
    #define SE_DEBUGBREAK __debugbreak()

    // Expands to the signature of the function in which the macro is located.
    #define SE_FUNCTION __FUNCSIG__
#endif // SE_COMPILER_MSVC

// The compiler is encouraged to issue a warning if the function return value is not stored/used.
#define NODISCARD [[nodiscard]]

// Suppreses warnings on unused entities.
#define MAYBE_UNUSED [[maybe_unused]]

// Represent hints to the compiler that the path of execution is more or less likely than the alternative.
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

// Expands to the number of elements stored in the given static array.
#define SE_ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))

// Constants regarding memory size units.
#define KiB (static_cast<usize>(1024))
#define MiB (1024 * KiB)
#define GiB (1024 * MiB)
