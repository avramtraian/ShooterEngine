// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

/**
 * PLATFORM MACROS.
 */

#ifndef SE_PLATFORM_WINDOWS
    #define SE_PLATFORM_WINDOWS 0
#endif // SE_PLATFORM_WINDOWS

/**
 * BUILD CONFIGURATION MACROS.
 */

#ifndef SE_CONFIGURATION_EDITOR_DEBUG
    #define SE_CONFIGURATION_EDITOR_DEBUG       0
#endif // SE_CONFIGURATION_EDITOR_DEBUG

#ifndef SE_CONFIGURATION_EDITOR_DEVELOPMENT
    #define SE_CONFIGURATION_EDITOR_DEVELOPMENT 0
#endif // SE_CONFIGURATION_EDITOR_DEVELOPMENT

#ifndef SE_CONFIGURATION_GAME_DEBUG
    #define SE_CONFIGURATION_GAME_DEBUG         0
#endif // SE_CONFIGURATION_GAME_DEBUG

#ifndef SE_CONFIGURATION_GAME_DEVELOPMENT
    #define SE_CONFIGURATION_GAME_DEVELOPMENT   0
#endif // SE_CONFIGURATION_GAME_DEVELOPMENT

#ifndef SE_CONFIGURATION_GAME_SHIPPING
    #define SE_CONFIGURATION_GAME_SHIPPING      0
#endif // SE_CONFIGURATION_GAME_SHIPPING

/* Target macros. */
#define SE_TARGET_EDITOR (SE_CONFIGURATION_EDITOR_DEBUG || SE_CONFIGURATION_EDITOR_DEVELOPMENT)
#define SE_TARGET_GAME   (SE_CONFIGURATION_GAME_DEBUG   || SE_CONFIGURATION_GAME_DEVELOPMENT || SE_CONFIGURATION_GAME_SHIPPING)

/* Configuration macros. */
#define SE_CONFIGURATION_DEBUG       (SE_CONFIGURATION_EDITOR_DEBUG       || SE_CONFIGURATION_GAME_DEBUG)
#define SE_CONFIGURATION_DEVELOPMENT (SE_CONFIGURATION_EDITOR_DEVELOPMENT || SE_CONFIGURATION_GAME_DEVELOPMENT)
#define SE_CONFIGURATION_SHIPPING    (SE_CONFIGURATION_GAME_SHIPPING)

/**
 * SHARED LIBRARY API SPECIFIERS.
 */

#if SE_PLATFORM_WINDOWS
    #define SE_API_SPECIFIER_EXPORT __declspec(dllexport)
    #define SE_API_SPECIFIER_IMPORT __declspec(dllimport)
#else
    #define SE_API_SPECIFIER_EXPORT
    #define SE_API_SPECIFIER_IMPORT
#endif // SE_PLATFORM_WINDOWS

#if SE_TARGET_EDITOR
    #ifdef SE_PROJECT_RUNTIME
        // DEPRECATED!
        #define SHOOTER_API SE_API_SPECIFIER_EXPORT
    #else
        // DEPRECATED!
        #define SHOOTER_API SE_API_SPECIFIER_IMPORT
    #endif // SE_PROJECT_RUNTIME
#else
    // DEPRECATED!
    #define SHOOTER_API
#endif // SE_TARGET_EDITOR

// NOTE(Traian): For now, 'ENGINE_API' is just a wrapper around the 'SHOOTER_API' but we plan to migrate
// all code using the 'SHOOTER_API' API specifier macro to the new 'ENGINE_API'. Always use this one!
#define ENGINE_API SHOOTER_API

#if SE_PLATFORM_WINDOWS
    // NOTE(Traian): Disable the warnings related to DLL specifiers.
    #pragma warning(disable : 4251)
    #pragma warning(disable : 4275)

    // NOTE(Traian): Disable warning C4200: nonstandard extension used: zero-sized array in struct/union
    #pragma warning(disable : 4200)
#endif // SE_PLATFORM_WINDOWS

/**
 * GENERAL-PURPOSE, PLATFORM AGNOSTIC, MACROS.
 */

#define FORCEINLINE            __forceinline
#define SE_PLATFORM_DEBUGBREAK __debugbreak()
#define SE_FUNCTION            __FUNCSIG__

#define NODISCARD    [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]
#define LIKELY       [[likely]]
#define UNLIKELY     [[unlikely]]

#define SE_ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))
#define SE_BIT(x)            (1 << (x))
