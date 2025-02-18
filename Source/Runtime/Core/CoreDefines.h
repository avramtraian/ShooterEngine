// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

//======================================================================================
// PLATFORM CONFIGURATION MACROS.
//======================================================================================
//
// NOTE: The platform and configurations macros are set by the build system.
// It is the responsability of the build system to ensure that they are set correctly.
//

#ifndef SE_PLATFORM_WIN64
    #define SE_PLATFORM_WIN64 0
#endif // SE_PLATFORM_WIN64

/* Ensure that at least one platform macro is set to 1.
 * Otherwise, the project configuration is wrong and a compiler error should be raised. */
#if !SE_PLATFORM_WIN64
    #error Unknown or unsupported platform!
#endif // Any supported platform.

//======================================================================================
// BUILD CONFIGURATION MACROS.
//======================================================================================

#ifndef SE_CONFIGURATION_EDITOR_DEBUG
    #define SE_CONFIGURATION_EDITOR_DEBUG 0
#endif // SE_CONFIGURATION_EDITOR_DEBUG

#ifndef SE_CONFIGURATION_EDITOR_DEVELOPMENT
    #define SE_CONFIGURATION_EDITOR_DEVELOPMENT 0
#endif // SE_CONFIGURATION_EDITOR_DEVELOPMENT

#ifndef SE_CONFIGURATION_GAME_DEBUG
    #define SE_CONFIGURATION_GAME_DEBUG 0
#endif // SE_CONFIGURATION_GAME_DEBUG

#ifndef SE_CONFIGURATION_GAME_DEVELOPMENT
    #define SE_CONFIGURATION_GAME_DEVELOPMENT 0
#endif // SE_CONFIGURATION_GAME_DEVELOPMENT

#ifndef SE_CONFIGURATION_GAME_SHIPPING
    #define SE_CONFIGURATION_GAME_SHIPPING 0
#endif // SE_CONFIGURATION_GAME_SHIPPING

#define SE_CONFIGURATION_DEBUG       (SE_CONFIGURATION_EDITOR_DEBUG       || SE_CONFIGURATION_GAME_DEBUG)
#define SE_CONFIGURATION_DEVELOPMENT (SE_CONFIGURATION_EDITOR_DEVELOPMENT || SE_CONFIGURATION_GAME_DEVELOPMENT)
#define SE_CONFIGURATION_SHIPPING    (SE_CONFIGURATION_GAME_SHIPPING)

/* Ensure that at least one build configuration macro is set to 1.
 * Otherwise, the project configuration is wrong and a compiler error should be raised. */
#if !SE_CONFIGURATION_DEBUG && !SE_CONFIGURATION_DEVELOPMENT && !SE_CONFIGURATION_SHIPPING
    #error Unknown or unsupported build configuration!
#endif // Any supported build configuration.

#define SE_TARGET_EDITOR (SE_CONFIGURATION_EDITOR_DEBUG || SE_CONFIGURATION_EDITOR_DEVELOPMENT)
#define SE_TARGET_GAME   (SE_CONFIGURATION_GAME_DEBUG   || SE_CONFIGURATION_GAME_DEVELOPMENT   || SE_CONFIGURATION_GAME_SHIPPING)

/* Ensure that at least one build configuration macro is set to 1.
 * Otherwise, the project configuration is wrong and a compiler error should be raised. */
#if !SE_TARGET_EDITOR && !SE_TARGET_GAME
    #error Unknown or unsupported build configuration target!
#endif // Any supported build configuration target.

//======================================================================================
// COMPILER CONFIGURATION MACROS.
//======================================================================================

#if !defined(_MSC_BUILD) || defined(__clang__)
    #error The engine can only be compiled using the MSVC toolchain!
#endif // _MSC_BUILD

//======================================================================================
// UTILITY (GENERAL PURPOSE) MACROS.
//======================================================================================

#define ALWAYS_INLINE             __forceinline
#define SE_PLATFORM_DEBUGBREAK    __debugbreak()
#define SE_FUNCTION               __FUNCSIG__
#define NODISCARD                 [[nodiscard]]
#define MAYBE_UNUSED              [[maybe_unused]]
#define LIKELY                    [[likely]]
#define UNLIKELY                  [[unlikely]]
#define SE_ARRAY_COUNT(x)         (sizeof(x) / sizeof((x)[0]))
#define SE_OFFSET_OF(type, field) ((usize) & (((type*)0)->field))

#define SE_KILOBYTES(x) (static_cast<usize>(1024) * (x))
#define SE_MEGABYTES(x) (static_cast<usize>(1024) * SE_KILOBYTES(x))
#define SE_GIGABYTES(x) (static_cast<usize>(1024) * SE_MEGABYTES(x))

//======================================================================================
// API SPECIFIERS.
//======================================================================================

#if SE_PLATFORM_WIN64
    #define SE_API_SPECIFIER_EXPORT __declspec(dllexport)
    #define SE_API_SPECIFIER_IMPORT __declspec(dllimport)
#else
    #define SE_API_SPECIFIER_EXPORT
    #define SE_API_SPECIFIER_IMPORT
#endif // SE_PLATFORM_WIN64

#if SE_TARGET_EDITOR
    #ifdef SE_PROJECT_RUNTIME
        #define SHOOTER_API SE_API_SPECIFIER_EXPORT
    #else
        #define SHOOTER_API SE_API_SPECIFIER_IMPORT
    #endif // SE_PROJECT_RUNTIME
#else
    #define SHOOTER_API
#endif // SE_TARGET_EDITOR
