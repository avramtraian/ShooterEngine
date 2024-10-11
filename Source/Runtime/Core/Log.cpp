/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Core/Platform/Platform.h>

namespace SE
{

#define SE_CONSOLE_TEXT_COLOR_INDEX       (0)
#define SE_CONSOLE_BACKGROUND_COLOR_INDEX (1)

// clang-format off
static Platform::ConsoleColor s_console_color_table[5][2] = {
    { Platform::ConsoleColor::DarkGray,    Platform::ConsoleColor::Black },
    { Platform::ConsoleColor::Green,       Platform::ConsoleColor::Black },
    { Platform::ConsoleColor::LightYellow, Platform::ConsoleColor::Black },
    { Platform::ConsoleColor::LightRed,    Platform::ConsoleColor::Black },
    { Platform::ConsoleColor::White,       Platform::ConsoleColor::Red   },
};
// clang-format on

static StringView s_severity_text_table[5] = { "TRACE"sv, "INFO"sv, "WARN"sv, "ERROR"sv, "FATAL"sv };
static StringView s_severity_padding_table[5] = { " "sv, "  "sv, "  "sv, " "sv, " "sv };

void Logger::log_message(Severity::Type severity, StringView message)
{
    Optional<String> formatted_message =
        format("[14:28:35][{}]:{}{}\n"sv, s_severity_text_table[(u8)(severity)], s_severity_padding_table[(u8)(severity)], message);
    if (!formatted_message.has_value())
        return;

    Platform::ConsoleColor text_color = s_console_color_table[severity][SE_CONSOLE_TEXT_COLOR_INDEX];
    Platform::ConsoleColor background_color = s_console_color_table[severity][SE_CONSOLE_BACKGROUND_COLOR_INDEX];
    Platform::write_to_console(formatted_message.value().view(), text_color, background_color);
}

void Logger::log_tagged_message(Severity::Type severity, StringView tag, StringView message)
{
    Optional<String> formatted_message =
        format("[14:28:35][{}][{}]:{}{}\n"sv, s_severity_text_table[severity], tag, s_severity_padding_table[severity], message);
    if (!formatted_message.has_value())
        return;

    Platform::ConsoleColor text_color = s_console_color_table[severity][SE_CONSOLE_TEXT_COLOR_INDEX];
    Platform::ConsoleColor background_color = s_console_color_table[severity][SE_CONSOLE_BACKGROUND_COLOR_INDEX];
    Platform::write_to_console(formatted_message.value().view(), text_color, background_color);
}

} // namespace SE
