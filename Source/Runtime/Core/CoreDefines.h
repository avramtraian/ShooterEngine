/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

//========================================================================================================================================//
//---------------------------------------------------------- COMPILER DETECTION ----------------------------------------------------------//
//========================================================================================================================================//

#ifdef __clang__
    #define SE_COMPILER_CLANG 1
#else
    #ifdef _MSC_BUILD
        #define SE_COMPILER_MSVC 1
    #endif // _MSC_BUILD
#endif // __clang__

#ifndef SE_COMPILER_MSVC
    #define SE_COMPILER_MSVC 0
#endif // SE_COMPILER_MSVC

#ifndef SE_COMPILER_CLANG
    #define SE_COMPILER_CLANG 0
#endif // SE_COMPILER_CLANG

#if !SE_COMPILER_CLANG && !SE_COMPILER_MSVC
    #error Unknown or unsupported compiler!
#endif // Any compiler.

#define SE_COMPILER_COUNT (SE_COMPILER_CLANG + SE_COMPILER_MSVC)
#if (SE_COMPILER_COUNT > 1)
    #error Multiple compilers were detected!
#endif // There are multiple compiler macros set to 1.

//========================================================================================================================================//
//------------------------------------------------------- PLATFORM OS DETECTION ----------------------------------------------------------//
//========================================================================================================================================//

#if defined(_WIN32)
    #define SE_PLATFORM_WINDOWS 1
#endif // defined(_WIN32)

#if defined(__linux__)
    #define SE_PLATFORM_LINUX 1
#endif // defined(__linux__)

#ifndef SE_PLATFORM_WINDOWS
    #define SE_PLATFORM_WINDOWS 0
#endif // SE_PLATFORM_WINDOWS

#ifndef SE_PLATFORM_LINUX
    #define SE_PLATFORM_LINUX 0
#endif // SE_PLATFORM_LINUX

#ifndef SE_PLATFORM_MACOS
    #define SE_PLATFORM_MACOS 0
#endif // SE_PLATFORM_MACOS

#if !SE_PLATFORM_WINDOWS && !SE_PLATFORM_LINUX && !SE_PLATFORM_MACOS
    #error Unknown or unsupported platform operating system!
#endif // Any supported platform.

#define SE_PLATFORM_COUNT (SE_PLATFORM_WINDOWS + SE_PLATFORM_LINUX + SE_PLATFORM_MACOS)
#if (SE_PLATFORM_COUNT > 1)
    #error Multiple platform operating systems were detected!
#endif // There are multiple platform operating system macros set to 1.

//========================================================================================================================================//
//---------------------------------------------------- PLATFORM ARCHITECTURE DETECTION ---------------------------------------------------//
//========================================================================================================================================//

#if SE_COMPILER_MSVC
// NOTE: Check the architecture macros defined by the MSVC compiler.

    #if defined(_M_IX86)
        #define SE_PLATFORM_ARCHITECTURE_X32 1
    #endif // defined(_M_IX86)
    #if defined(_M_X64) || defined(_M_AMD64)
        #define SE_PLATFORM_ARCHITECTURE_X64 1
    #endif // defined(_M_X64) || defined(_M_AMD64)
    #if defined(_M_ARM)
        #define SE_PLATFORM_ARCHITECTURE_ARM32 1
    #endif // defined(_M_ARM)
    #if defined(_M_ARM64)
        #define SE_PLATFORM_ARCHITECTURE_ARM64 1
    #endif // defined(_M_ARM64)

#elif SE_COMPILER_CLANG
// NOTE: Check the architecture macros defined by Clang.

    #if defined(__i386__)
        #define SE_PLATFORM_ARCHITECTURE_X32 1
    #endif // defined(__i386__)
    #if defined(__x86_64__) || defined(__amd64__)
        #define SE_PLATFORM_ARCHITECTURE_X64 1
    #endif // defined(__x86_64__) || defined(__amd64__)
    #if defined(__arm__)
        #define SE_PLATFORM_ARCHITECTURE_ARM32 1
    #endif // defined(__arm__)
    #if defined(__aarch64__)
        #define SE_PLATFORM_ARCHITECTURE_ARM64 1
    #endif // defined(__aarch64__)

#endif // Supported compilers.

#ifndef SE_PLATFORM_ARCHITECTURE_X32
    #define SE_PLATFORM_ARCHITECTURE_X32 0
#endif // SE_PLATFORM_ARCHITECTURE_X32

#ifndef SE_PLATFORM_ARCHITECTURE_X64
    #define SE_PLATFORM_ARCHITECTURE_X64 0
#endif // SE_PLATFORM_ARCHITECTURE_X64

#ifndef SE_PLATFORM_ARCHITECTURE_ARM32
    #define SE_PLATFORM_ARCHITECTURE_ARM32 0
#endif // SE_PLATFORM_ARCHITECTURE_ARM32

#ifndef SE_PLATFORM_ARCHITECTURE_ARM64
    #define SE_PLATFORM_ARCHITECTURE_ARM64 0
#endif // SE_PLATFORM_ARCHITECTURE_ARM64

#if !SE_PLATFORM_ARCHITECTURE_X32 && !SE_PLATFORM_ARCHITECTURE_X64 && !SE_PLATFORM_ARCHITECTURE_ARM32 && !SE_PLATFORM_ARCHITECTURE_ARM64
    #error Unnkown or unsupported platform architecture!
#endif // Any supported architecture.

#define SE_PLATFORM_ARCHITECTURE_COUNT \
    (SE_PLATFORM_ARCHITECTURE_X32 + SE_PLATFORM_ARCHITECTURE_X64 + SE_PLATFORM_ARCHITECTURE_ARM32 + SE_PLATFORM_ARCHITECTURE_ARM64)
#if (SE_PLATFORM_ARCHITECTURE_COUNT > 1)
    #error Multiple platform architectures were detected!
#endif // There are multiple platform architecture macros set to 1.

//========================================================================================================================================//
//---------------------------------------------------- CONFIGURATION AND TARGET MACROS ---------------------------------------------------//
//========================================================================================================================================//

#ifndef SE_CONFIGURATION_DEBUG
    #define SE_CONFIGURATION_DEBUG 0
#endif // SE_CONFIGURATION_DEBUG

#ifndef SE_CONFIGURATION_DEVELOPMENT
    #define SE_CONFIGURATION_DEVELOPMENT 0
#endif // SE_CONFIGURATION_DEVELOPMENT

#ifndef SE_CONFIGURATION_SHIPPING
    #define SE_CONFIGURATION_SHIPPING 0
#endif // SE_CONFIGURATION_SHIPPING

#ifndef SE_TARGET_RUNTIME_STATIC
    #define SE_TARGET_RUNTIME_STATIC 0
#endif // SE_TARGET_RUNTIME_STATIC

#ifndef SE_TARGET_RUNTIME_SHARED
    #define SE_TARGET_RUNTIME_SHARED 0
#endif // SE_TARGET_RUNTIME_SHARED

#ifndef SE_TARGET_GAME_STANDALONE
    #define SE_TARGET_GAME_STANDALONE 0
#endif // SE_TARGET_GAME_STANDALONE

#ifndef SE_TARGET_GAME_PIE
    #define SE_TARGET_GAME_PIE 0
#endif // SE_TARGET_GAME_PIE

#ifndef SE_TARGET_EDITOR
    #define SE_TARGET_EDITOR 0
#endif // SE_TARGET_EDITOR

#if !SE_TARGET_RUNTIME_STATIC && !SE_TARGET_RUNTIME_SHARED && !SE_TARGET_GAME_STANDALONE && !SE_TARGET_GAME_PIE && !SE_TARGET_EDITOR
    #error Unknown build target specified!
#endif // Any supported target.

#define SE_TARGET_COUNT (SE_TARGET_RUNTIME_STATIC + SE_TARGET_RUNTIME_SHARED + SE_TARGET_GAME_STANDALONE + SE_TARGET_GAME_PIE + SE_TARGET_EDITOR)
#if (SE_TARGET_COUNT > 1)
    #error Multiple build target specified at the same time!
#endif // There are multiple build target macros set to 1.

#if SE_TARGET_RUNTIME_STATIC || SE_TARGET_RUNTIME_SHARED
    #define SE_TARGET_RUNTIME 0
#else
    #define SE_TARGET_RUNTIME 0
#endif // SE_TARGET_RUNTIME_STATIC || SE_TARGET_RUNTIME_SHARED

#if SE_TARGET_GAME_STANDALONE || SE_TARGET_GAME_PIE
    #define SE_TARGET_GAME 0
#else
    #define SE_TARGET_GAME 0
#endif // SE_TARGET_GAME_STANDALONE || SE_TARGET_GAME_PIE

//========================================================================================================================================//
//------------------------------------------------------------ UTILITY MACROS ------------------------------------------------------------//
//========================================================================================================================================//

#define NODISCARD    [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]
#define LIKELY       [[likely]]
#define UNLIKELY     [[unlikely]]

#if SE_COMPILER_CLANG
    #define ALWAYS_INLINE          __attribute__((always_inline)) inline
    #define SE_FUNCTION            __PRETTY_FUNCTION__
    #define SE_PLATFORM_DEBUGBREAK __builtin_trap()
#elif SE_COMPILER_MSVC
    #define ALWAYS_INLINE          __forceinline
    #define SE_FUNCTION            __FUNCSIG__
    #define SE_PLATFORM_DEBUGBREAK __debugbreak()
#endif // Supported compiler

#define SE_IMPL_STRINGIFY(x)      #x
#define SE_IMPL_CONCATENATE(x, y) x##y

#define SE_STRINGIFY(x)           SE_IMPL_STRINGIFY(x)
#define SE_CONCATENATE(x, y)      SE_IMPL_CONCATENATE(x, y)
#define SE_ARRAY_COUNT(x)         (sizeof(x) / sizeof((x)[0]))
#define SE_BIT(x)                 (1 << (x))

//========================================================================================================================================//
//--------------------------------------------------------- EXPORT API SPECIFIERS --------------------------------------------------------//
//========================================================================================================================================//

#if SE_PLATFORM_WINDOWS
    #define SE_API_SPECIFIER_EXPORT __declspec(dllexport)
    #define SE_API_SPECIFIER_IMPORT __declspec(dllimport)
#else
    #define SE_API_SPECIFIER_EXPORT __attribute__((visibility("default")))
    #define SE_API_SPECIFIER_IMPORT __attribute__((visibility("default")))
#endif // SE_PLATFORM_WINDOWS

#if SE_TARGET_RUNTIME_SHARED
    #define RUNTIME_API SE_API_SPECIFIER_EXPORT
#endif // SE_TARGET_RUNTIME_SHARED

#if SE_TARGET_EDITOR || SE_TARGET_GAME_PIE
    #define RUNTIME_API SE_API_SPECIFIER_IMPORT
#endif // SE_TARGET_EDITOR || SE_TARGET_GAME_PIE

#ifndef RUNTIME_API
    #define RUNTIME_API
#endif // RUNTIME_API

// DEPRECATED: These macro are here only for compatibility reasons - no newly written code should use them.
#define SHOOTER_API RUNTIME_API
#define ENGINE_API  RUNTIME_API
#define FORCEINLINE ALWAYS_INLINE
