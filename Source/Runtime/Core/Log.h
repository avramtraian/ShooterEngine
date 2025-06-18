// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

/* TODO: Required for formatting strings passed to the logging macros.
 * Implement a custom string formatting functions and use it intead of depending on libc. */
#include <cstdio>

namespace SE
{

enum class LogCategory : uint8
{
    Debug = 0,
    Trace = 1,
    Info  = 2,
    Warn  = 3,
    Error = 4,
};

class Logger
{
    SE_MAKE_SINGLETON(Logger);

public:
    SHOOTER_API static bool initialize();
    SHOOTER_API static void shutdown();

    SHOOTER_API static void submit(LogCategory category, const char* message);

    template<typename... Args>
    FORCEINLINE static void submit(LogCategory category, const char* message, Args&&... args)
    {
        static char s_formatted_message_buffer[4096] = {};
        const int written_byte_count = sprintf_s(s_formatted_message_buffer, message, forward<Args>(args)...);
        if (written_byte_count < 0)
        {
            /* A formatting error occured. We should probably propagate the error to the caller instead
             * of just silently ignoring the submition. */
            return;
        }

        Logger::submit(category, s_formatted_message_buffer);
    }
};

}

#if SE_CONFIGURATION_DEBUG
    #define SE_ENABLE_LOG_DEBUG 0
    #define SE_ENABLE_LOG_TRACE 1
    #define SE_ENABLE_LOG_INFO  1
    #define SE_ENABLE_LOG_WARN  1
    #define SE_ENABLE_LOG_ERROR 1
#endif // SE_CONFIGURATION_DEBUG

#if SE_CONFIGURATION_DEVELOPMENT
    #define SE_ENABLE_LOG_DEBUG 0
    #define SE_ENABLE_LOG_TRACE 0
    #define SE_ENABLE_LOG_INFO  1
    #define SE_ENABLE_LOG_WARN  1
    #define SE_ENABLE_LOG_ERROR 1
#endif // SE_CONFIGURATION_DEVELOPMENT

#if SE_CONFIGURATION_SHIPPING
    #define SE_ENABLE_LOG_DEBUG 0
    #define SE_ENABLE_LOG_TRACE 0
    #define SE_ENABLE_LOG_INFO  0
    #define SE_ENABLE_LOG_WARN  0
    #define SE_ENABLE_LOG_ERROR 1
#endif // SE_CONFIGURATION_SHIPPING

#if SE_ENABLE_LOG_DEBUG
    #define SE_LOG_DEBUG(...) { ::SE::Logger::submit(::SE::LogCategory::Debug, __VA_ARGS__); }
#else
    #define SE_LOG_DEBUG(...)
#endif // SE_ENABLE_LOG_DEBUG

#if SE_ENABLE_LOG_TRACE
    #define SE_LOG_TRACE(...) { ::SE::Logger::submit(::SE::LogCategory::Trace, __VA_ARGS__); }
#else
    #define SE_LOG_TRACE(...)
#endif // SE_ENABLE_LOG_TRACE

#if SE_ENABLE_LOG_INFO
    #define SE_LOG_INFO(...) { ::SE::Logger::submit(::SE::LogCategory::Info, __VA_ARGS__); }
#else
    #define SE_LOG_INFO(...)
#endif // SE_ENABLE_LOG_INFO

#if SE_ENABLE_LOG_WARN
    #define SE_LOG_WARN(...) { ::SE::Logger::submit(::SE::LogCategory::Warn, __VA_ARGS__); }
#else
    #define SE_LOG_WARN(...)
#endif // SE_ENABLE_LOG_WARN

#if SE_ENABLE_LOG_ERROR
    #define SE_LOG_ERROR(...) { ::SE::Logger::submit(::SE::LogCategory::Error, __VA_ARGS__); }
#else
    #define SE_LOG_ERROR(...)
#endif // SE_ENABLE_LOG_ERROR
