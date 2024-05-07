/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreDefines.h"

#if SE_CONFIGURATION_DEBUG || SE_CONFIGURATION_DEVELOPMENT
    #define SE_ASSERT(...)  \
        if (!(__VA_ARGS__)) \
        {                   \
            SE_DEBUGBREAK;  \
        }
#else
    #define SE_ASSERT(...) // Exclude from build.
#endif // Debug or Development configurations.

#if SE_CONFIGURATION_DEBUG
    #define SE_ASSERT_DEBUG(...) \
        if (!(__VA_ARGS__))      \
        {                        \
            SE_DEBUGBREAK;       \
        }
#else
    #define SE_ASSERT_DEBUG(...) // Exclude from build.
#endif // SE_CONFIGURATION_DEBUG