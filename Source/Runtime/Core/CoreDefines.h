/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#ifndef SE_PLATFORM_WINDOWS
    #define SE_PLATFORM_WINDOWS 0
#endif // SE_PLATFORM_WINDOWS

#ifndef SE_PLATFORM_MACOS
    #define SE_PLATFORM_MACOS 0
#endif // SE_PLATFORM_MACOS

#ifndef SE_PLATFORM_LINUX
    #define SE_PLATFORM_LINUX 0
#endif // SE_PLATFORM_LINUX

#if !SE_PLATFORM_WINDOWS && !SE_PLATFORM_MACOS && !SE_PLATFORM_LINUX
    #error Unknown or unsupported platform!
#endif // No known platform.

#ifndef SE_CONFIGURATION_DEBUG
    #define SE_CONFIGURATION_DEBUG 0
#endif // SE_CONFIGURATION_DEBUG

#ifndef SE_CONFIGURATION_DEVELOPMENT
    #define SE_CONFIGURATION_DEVELOPMENT 0
#endif // SE_CONFIGURATION_DEVELOPMENT

#ifndef SE_CONFIGURATION_SHIPPING
    #define SE_CONFIGURATION_SHIPPING 0
#endif // SE_CONFIGURATION_SHIPPING

#if !SE_CONFIGURATION_DEBUG && !SE_CONFIGURATION_DEVELOPMENT && !SE_CONFIGURATION_SHIPPING
    #error No build configuration was specified!
#endif // No configuration specified.

#ifdef _MSC_BUILD
    #define SE_COMPILER_MSVC  1
    #define SE_COMPILER_GCC   0
    #define SE_COMPILER_CLANG 0
#else
    #define SE_COMPILER_MSVC  0
    #define SE_COMPILER_GCC   0
    #define SE_COMPILER_CLANG 0
#endif // _MSC_BUILD

#if !SE_COMPILER_MSVC
    #error Unknown or unsupported compiler!
#endif // No known compiler.

#define SE_LINE __LINE__
#define SE_FILE __FILE__
#define SE_DATE __DATE__

#define NODISCARD    [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]
#define LIKELY       [[likely]]
#define UNLIKELY     [[unlikely]]

#if SE_COMPILER_MSVC
    #define SE_FUNCTION   __FUNCSIG__
    #define ALWAYS_INLINE __forceinline
#endif // Compiler switch.