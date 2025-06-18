// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

enum class AssertionKind : uint8
{
    Assert,
    Check,
    Ensure,
};

SHOOTER_API void on_assertion_failed(AssertionKind kind, const char* expression, const char* file, const char* function, uint32 line);

}

#if SE_CONFIGURATION_DEBUG
    #define SE_ENABLE_ASSERTIONS_ASSERT 1
    #define SE_ENABLE_ASSERTIONS_CHECK  1
    #define SE_ENABLE_ASSERTIONS_ENSURE 1
#endif // SE_CONFIGURATION_DEBUG

#if SE_CONFIGURATION_DEVELOPMENT
    #define SE_ENABLE_ASSERTIONS_ASSERT 0
    #define SE_ENABLE_ASSERTIONS_CHECK  1
    #define SE_ENABLE_ASSERTIONS_ENSURE 1
#endif // SE_CONFIGURATION_DEVELOPMENT

#if SE_CONFIGURATION_SHIPPING
    #define SE_ENABLE_ASSERTIONS_ASSERT 0
    #define SE_ENABLE_ASSERTIONS_CHECK  0
    #define SE_ENABLE_ASSERTIONS_ENSURE 1
#endif // SE_CONFIGURATION_SHIPPING

#if SE_ENABLE_ASSERTIONS_ASSERT
    #define SE_ASSERT(...)                                                                                         \
        if (!(__VA_ARGS__))                                                                                        \
        {                                                                                                          \
            ::SE::on_assertion_failed(::SE::AssertionKind::Assert, #__VA_ARGS__, __FILE__, SE_FUNCTION, __LINE__); \
            SE_PLATFORM_DEBUGBREAK;                                                                                \
        }
#else
    #define SE_ASSERT(...)
#endif // SE_ENABLE_ASSERTIONS_ASSERT

#if SE_ENABLE_ASSERTIONS_CHECK
    #define SE_CHECK(...)                                                                                         \
        if (!(__VA_ARGS__))                                                                                       \
        {                                                                                                         \
            ::SE::on_assertion_failed(::SE::AssertionKind::Check, #__VA_ARGS__, __FILE__, SE_FUNCTION, __LINE__); \
            SE_PLATFORM_DEBUGBREAK;                                                                               \
        }
#else
    #define SE_CHECK(...)
#endif // SE_ENABLE_ASSERTIONS_CHECK

#if SE_ENABLE_ASSERTIONS_ENSURE
    #define SE_ENSURE(...)                                                                                         \
        if (!(__VA_ARGS__))                                                                                        \
        {                                                                                                          \
            ::SE::on_assertion_failed(::SE::AssertionKind::Ensure, #__VA_ARGS__, __FILE__, SE_FUNCTION, __LINE__); \
            SE_PLATFORM_DEBUGBREAK;                                                                                \
        }
#else
    #define SE_ENSURE(...)
#endif // SE_ENABLE_ASSERTIONS_ENSURE

/* Crashes the program when encountered. Should only be used when the program is an unrecovarable state. */
#define SE_ASSERT_NOT_REACHED SE_PLATFORM_DEBUGBREAK
