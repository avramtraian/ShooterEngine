// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/Log.h>

namespace SE
{

bool Logger::initialize()
{
    return true;
}

void Logger::shutdown()
{}

void Logger::submit(LogCategory category, const char* message)
{
    static constexpr const char* s_category_table[] =
    {
        "DEBUG", /* 0 */
        "TRACE", /* 1 */
        "INFO",  /* 2 */
        "WARN",  /* 3 */
        "ERROR", /* 4 */
    };

    static constexpr const char* s_padding_table[] =
    {
        "",  /* 0 (Debug) */
        "",  /* 1 (Trace) */
        " ", /* 2 (Info)  */
        " ", /* 3 (Warn)  */
        "",  /* 4 (Error) */
    };

    static char s_log_message_buffer[4096] = {};
    const usize category_index = static_cast<usize>(category);

    const int written_byte_count = sprintf_s(
        s_log_message_buffer,
        "[%s]: %s%s\n",
        s_category_table[category_index], s_padding_table[category_index], message
    );

    SE_CHECK(written_byte_count >= 0 && written_byte_count < sizeof(s_log_message_buffer));
    printf_s(s_log_message_buffer);
}

}
