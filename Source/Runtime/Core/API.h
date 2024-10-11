/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreDefines.h>

#if SE_PLATFORM_WINDOWS
    #if SE_COMPILER_MSVC
        #define SE_API_SPECIFIER_EXPORT __declspec(dllexport)
        #define SE_API_SPECIFIER_IMPORT __declspec(dllimport)
    #endif // Compiler switch.
#endif // Platform switch.

#if SE_CONFIGURATION_TARGET_EDITOR
    #ifdef SE_EXPORT_ENGINE_API
        #define SHOOTER_API SE_API_SPECIFIER_EXPORT
    #else
        #define SHOOTER_API SE_API_SPECIFIER_IMPORT
    #endif // SE_EXPORT_ENGINE_API
#else
    #define SHOOTER_API
#endif // SE_CONFIGURATION_TARGET_EDITOR
