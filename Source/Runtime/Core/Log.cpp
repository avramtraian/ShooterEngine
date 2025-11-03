// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/Log.h>

namespace SE
{

bool Logger::Initialize()
{
    return true;
}

void Logger::Shutdown()
{}

void Logger::Submit(LogCategory category, const char* message)
{
    static constexpr const char* s_CategoryTable[] =
    {
        "DEBUG", /* 0 */
        "TRACE", /* 1 */
        "INFO",  /* 2 */
        "WARN",  /* 3 */
        "ERROR", /* 4 */
    };

    static constexpr const char* s_PaddingTable[] =
    {
        "",  /* 0 (Debug) */
        "",  /* 1 (Trace) */
        " ", /* 2 (Info)  */
        " ", /* 3 (Warn)  */
        "",  /* 4 (Error) */
    };

    static char s_LogMessageBuffer[4096] = {};
    const usize categoryIndex = static_cast<usize>(category);

    MAYBE_UNUSED const int writtenByteCount = sprintf_s(
        s_LogMessageBuffer,
        "[%s]: %s%s\n",
        s_CategoryTable[categoryIndex], s_PaddingTable[categoryIndex], message
    );

    SE_CHECK(writtenByteCount >= 0 && writtenByteCount < sizeof(s_LogMessageBuffer));
    printf_s(s_LogMessageBuffer);
}

}
