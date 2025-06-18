// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/StringView.h>

namespace SE
{

StringView StringView::FromUTF8(const char* nullTerminatedCharacters)
{
    const char* stringIterator = nullTerminatedCharacters;
    while (*stringIterator++) {}
    return StringView::FromUTF8(nullTerminatedCharacters, (stringIterator - nullTerminatedCharacters) - 1);
}

}
