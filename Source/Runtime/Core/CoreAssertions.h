// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

#if SE_CONFIGURATION_DEBUG
    #define SE_CHECKION_ENABLE_ASSERTS 1
    #define SE_CHECKION_ENABLE_CHECKS  1
    #define SE_CHECKION_ENABLE_ENSURES 1
#endif // SE_CONFIGURATION_DEBUG

#if SE_CONFIGURATION_DEVELOPMENT
    #define SE_CHECKION_ENABLE_ASSERTS 0
    #define SE_CHECKION_ENABLE_CHECKS  1
    #define SE_CHECKION_ENABLE_ENSURES 1
#endif // SE_CONFIGURATION_DEBUG

#if SE_CONFIGURATION_SHIPPING
    #define SE_CHECKION_ENABLE_ASSERTS 0
    #define SE_CHECKION_ENABLE_CHECKS  0
    #define SE_CHECKION_ENABLE_ENSURES 1
#endif // SE_CONFIGURATION_DEBUG

namespace SE
{

enum class AssertionKind : u8
{
    Assert,
    Check,
    Ensure,
};

SHOOTER_API void on_assertion_failed(AssertionKind kind, const char* expression, const char* filename, const char* function, u32 line);

}

#if SE_CHECKION_ENABLE_ASSERTS
    #define SE_ASSERT(...)                                                                                         \
        if (!(__VA_ARGS__))                                                                                        \
        {                                                                                                          \
            ::SE::on_assertion_failed(::SE::AssertionKind::Assert, #__VA_ARGS__, __FILE__, SE_FUNCTION, __LINE__); \
            SE_PLATFORM_DEBUGBREAK;                                                                                \
        }
#else
    #define SE_ASSERT(...)
#endif // SE_CHECKION_ENABLE_ASSERTS

#if SE_CHECKION_ENABLE_CHECKS
    #define SE_CHECK(...)                                                                                         \
        if (!(__VA_ARGS__))                                                                                       \
        {                                                                                                         \
            ::SE::on_assertion_failed(::SE::AssertionKind::Check, #__VA_ARGS__, __FILE__, SE_FUNCTION, __LINE__); \
            SE_PLATFORM_DEBUGBREAK;                                                                               \
        }
#else
    #define SE_CHECK(...)
#endif // SE_CHECKION_ENABLE_CHECKS

#if SE_CHECKION_ENABLE_ENSURES
    #define SE_ENSURE(...)                                                                                         \
        if (!(__VA_ARGS__))                                                                                        \
        {                                                                                                          \
            ::SE::on_assertion_failed(::SE::AssertionKind::Ensure, #__VA_ARGS__, __FILE__, SE_FUNCTION, __LINE__); \
            SE_PLATFORM_DEBUGBREAK;                                                                                \
        }
#else
    #define SE_ENSURE(...)
#endif // SE_CHECKION_ENABLE_ENSURES
