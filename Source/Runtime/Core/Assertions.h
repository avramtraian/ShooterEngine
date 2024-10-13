/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreDefines.h>

#if SE_CONFIGURATION_DEBUG
    #define SE_ASSERTION_ENABLE_DEBUG_ASSERT 1
    #define SE_ASSERTION_ENABLE_ASSERT       1
    #define SE_ASSERTION_ENABLE_VERIFY       1
#endif // SE_CONFIGURATION_DEBUG

#if SE_CONFIGURATION_DEVELOPMENT
    #define SE_ASSERTION_ENABLE_DEBUG_ASSERT 0
    #define SE_ASSERTION_ENABLE_ASSERT       1
    #define SE_ASSERTION_ENABLE_VERIFY       1
#endif // SE_CONFIGURATION_DEBUG

#if SE_CONFIGURATION_SHIPPING
    #define SE_ASSERTION_ENABLE_DEBUG_ASSERT 0
    #define SE_ASSERTION_ENABLE_ASSERT       0
    #define SE_ASSERTION_ENABLE_VERIFY       1
#endif // SE_CONFIGURATION_DEBUG

#if SE_ASSERTION_ENABLE_DEBUG_ASSERT
    #define SE_DEBUG_ASSERT(...) \
        if (!(__VA_ARGS__))      \
        {                        \
            SE_DEBUGBREAK;       \
        }
#else
    #define SE_DEBUG_ASSERT(...) // Exclude from build.
#endif // SE_ASSERTION_ENABLE_DEBUG_ASSERT

#if SE_ASSERTION_ENABLE_ASSERT
    #define SE_ASSERT(...)  \
        if (!(__VA_ARGS__)) \
        {                   \
            SE_DEBUGBREAK;  \
        }
#else
    #define SE_ASSERT(...) // Exclude from build.
#endif // SE_ASSERTION_ENABLE_ASSERT

#if SE_ASSERTION_ENABLE_VERIFY
    #define SE_VERIFY(...)  \
        if (!(__VA_ARGS__)) \
        {                   \
            SE_DEBUGBREAK;  \
        }
#else
    #define SE_VERIFY(...) // Exclude from build.
#endif // SE_ASSERTION_ENABLE_VERIFY
