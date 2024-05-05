/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/Format.h"
#include "Core/Containers/StringView.h"
#include "Core/CoreTypes.h"
#include "Core/EngineAPI.h"

namespace SE {

class Logger {
public:
    struct Severity {
        enum Type : u8 {
            Trace = 0,
            Info = 1,
            Warn = 2,
            Error = 3,
            Fatal = 4,

            EnumCount
        };
    };

public:
    SHOOTER_API static void log_message(Severity::Type severity, StringView message);
    SHOOTER_API static void log_tagged_message(Severity::Type severity, StringView tag, StringView message);

    template<typename... Args>
    ALWAYS_INLINE static void log_message(Severity::Type severity, StringView message, Args&&... args)
    {
        Optional<String> formatted_message = format(message, forward<Args>(args)...);
        if (!formatted_message.has_value())
            return;
        log_message(severity, formatted_message.value().view());
    }

    template<typename... Args>
    ALWAYS_INLINE static void
    log_tagged_message(Severity::Type severity, StringView tag, StringView message, Args&&... args)
    {
        Optional<String> formatted_message = format(message, forward<Args>(args)...);
        if (!formatted_message.has_value())
            return;
        log_message(severity, tag, formatted_message.value().view());
    }
};

} // namespace SE

#define SE_LOG_TRACE(...) ::SE::Logger::log_message(::SE::Logger::Severity::Trace, __VA_ARGS__)
#define SE_LOG_INFO(...)  ::SE::Logger::log_message(::SE::Logger::Severity::Info, __VA_ARGS__)
#define SE_LOG_WARN(...)  ::SE::Logger::log_message(::SE::Logger::Severity::Warn, __VA_ARGS__)
#define SE_LOG_ERROR(...) ::SE::Logger::log_message(::SE::Logger::Severity::Error, __VA_ARGS__)
#define SE_LOG_FATAL(...) ::SE::Logger::log_message(::SE::Logger::Severity::Fatal, __VA_ARGS__)

#define SE_LOG_TAG_TRACE(tag, ...) ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Trace, tag, __VA_ARGS__)
#define SE_LOG_TAG_INFO(tag, ...)  ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Info, tag, __VA_ARGS__)
#define SE_LOG_TAG_WARN(tag, ...)  ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Warn, tag, __VA_ARGS__)
#define SE_LOG_TAG_ERROR(tag, ...) ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Error, tag, __VA_ARGS__)
#define SE_LOG_TAG_FATAL(tag, ...) ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Fatal, tag, __VA_ARGS__)