// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/Log.h>

namespace SE
{

void on_assertion_failed(AssertionKind kind, const char* expression, const char* file, const char* function, u32 line)
{
    /* NOTE: The logging system might actually not be available, or even worse, the logging system is where
     * the assertion comes from. Using the logging system might cause an infinite recursion loop! */

    const char* assertion_kind_string = "(UNKNOWN)";
    switch (kind)
    {
        case AssertionKind::Assert: assertion_kind_string = "ASSERT"; break;
        case AssertionKind::Check:  assertion_kind_string = "CHECK";  break;
        case AssertionKind::Ensure: assertion_kind_string = "ENSURE"; break;
        default: break;
    }

    SE_LOG_ERROR("RUNTIME %s HAS FAILED:", assertion_kind_string);
    SE_LOG_ERROR("  - Expression: %s", expression);
    SE_LOG_ERROR("  - File:       %s", file);
    SE_LOG_ERROR("  - Function:   %s", function);
    SE_LOG_ERROR("  - Line:       %u", line);

     /* TODO: Open a pop-up window that displays the information submitted to the console above.
      * Probably implementing an abstraction layer for opening such pop-up windows would be overkill,
      * and thus a simple #if statement/switch would be enough. */
}

}
