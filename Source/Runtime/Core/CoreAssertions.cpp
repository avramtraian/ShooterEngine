// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/Log.h>

namespace SE
{

void on_assertion_failed(AssertionKind kind, const char* expression, const char* filename, const char* function, u32 line)
{
    StringView kind_as_string;
    switch (kind)
    {
        case AssertionKind::Assert: kind_as_string = "ASSERT"sv; break;
        case AssertionKind::Check:  kind_as_string = "CHECK"sv;  break;
        case AssertionKind::Ensure: kind_as_string = "ENSURE"sv; break;
        default: kind_as_string = "(Unknown)"sv; break;
    }

    SE_LOG_FATAL("{} FAILED:", kind_as_string);
    SE_LOG_FATAL("    Expression: {}", expression);
    SE_LOG_FATAL("    File:       {}", filename);
    SE_LOG_FATAL("    Function:   {}", function);
    SE_LOG_FATAL("    Line:       {}", line);

    // TODO: Open a pop-up window that contains the information also submited to the console.
    // Implementing a platform abstraction layer for this pop-up would be excessive?
}

}
