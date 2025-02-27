// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/StringView.h>

namespace SE
{

StringView StringView::from_utf8(const char* null_terminated_characters)
{
    const char* string_it = null_terminated_characters;
    while (*string_it++) {}
    return StringView::from_utf8(null_terminated_characters, string_it - null_terminated_characters - 1);
}

}
