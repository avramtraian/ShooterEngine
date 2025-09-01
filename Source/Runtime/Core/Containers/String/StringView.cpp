// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/String/StringView.h>
#include <Runtime/Core/Hash/CityHash/city.h>

namespace SE
{

NODISCARD SHOOTER_API uint64 GetCityHash64(const char* characters, usize byteCount)
{
    const uint64 hashValue = CityHash64(characters, byteCount);
    return hashValue;
}

StringView StringView::FromUTF8(const char* characters, usize byteCount)
{
    StringView view;
    view.m_Characters = characters;
    view.m_ByteCount = byteCount;
    return view;
}

StringView StringView::FromUTF8(const char* nullTerminatedCharacters)
{
    const char* stringIterator = nullTerminatedCharacters;
    while (*stringIterator++) {}
    const usize byteCount = (stringIterator - nullTerminatedCharacters) - 1;
    return StringView::FromUTF8(nullTerminatedCharacters, byteCount);
}

}
