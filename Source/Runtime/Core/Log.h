/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/CoreTypes.h>
#include <Core/String/Format.h>
#include <Core/String/StringView.h>

namespace SE
{

class Logger
{
public:
    struct Severity
    {
        enum Type : u8
        {
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
    ALWAYS_INLINE static void log_tagged_message(Severity::Type severity, StringView tag, StringView message, Args&&... args)
    {
        Optional<String> formatted_message = format(message, forward<Args>(args)...);
        if (!formatted_message.has_value())
            return;
        log_tagged_message(severity, tag, formatted_message.value().view());
    }
};

} // namespace SE

#define SE_LOG_TRACE(message, ...) ::SE::Logger::log_message(::SE::Logger::Severity::Trace, message##sv, __VA_ARGS__)
#define SE_LOG_INFO(message, ...)  ::SE::Logger::log_message(::SE::Logger::Severity::Info, message##sv, __VA_ARGS__)
#define SE_LOG_WARN(message, ...)  ::SE::Logger::log_message(::SE::Logger::Severity::Warn, message##sv, __VA_ARGS__)
#define SE_LOG_ERROR(message, ...) ::SE::Logger::log_message(::SE::Logger::Severity::Error, message##sv, __VA_ARGS__)
#define SE_LOG_FATAL(message, ...) ::SE::Logger::log_message(::SE::Logger::Severity::Fatal, message##sv, __VA_ARGS__)

#define SE_LOG_TAG_TRACE(tag, message, ...) ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Trace, tag##sv, message##sv, __VA_ARGS__)
#define SE_LOG_TAG_INFO(tag, message, ...)  ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Info, tag##sv, message##sv, __VA_ARGS__)
#define SE_LOG_TAG_WARN(tag, message, ...)  ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Warn, tag##sv, message##sv, __VA_ARGS__)
#define SE_LOG_TAG_ERROR(tag, message, ...) ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Error, tag##sv, message##sv, __VA_ARGS__)
#define SE_LOG_TAG_FATAL(tag, message, ...) ::SE::Logger::log_tagged_message(::SE::Logger::Severity::Fatal, tag##sv, message##sv, __VA_ARGS__)
