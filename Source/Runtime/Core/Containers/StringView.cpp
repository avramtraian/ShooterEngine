/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/StringView.h>
#include <Core/Containers/Utf8.h>

namespace SE
{

StringView StringView::create_from_utf8(const char* characters, usize byte_count)
{
    MAYBE_UNUSED bool validity = UTF8::check_validity({ reinterpret_cast<ReadonlyBytes>(characters), byte_count });
    SE_ASSERT(validity);

    return unsafe_create_from_utf8(characters, byte_count);
}

StringView StringView::create_from_utf8(ReadonlyByteSpan characters_byte_span)
{
    return create_from_utf8(characters_byte_span.as<const char>().elements(), characters_byte_span.count());
}

StringView StringView::create_from_utf8(const char* null_terminated_characters)
{
    usize byte_count = UTF8::byte_count(reinterpret_cast<ReadonlyBytes>(null_terminated_characters));
    SE_ASSERT(byte_count != invalid_size && byte_count != 0);

    // NOTE: The number of bytes also includes the null-termination character.
    return unsafe_create_from_utf8(null_terminated_characters, byte_count - 1);
}

usize StringView::find(char ascii_character) const
{
    for (usize offset = 0; offset < m_byte_count; ++offset)
    {
        if (m_characters[offset] == ascii_character)
        {
            return offset;
        }
    }

    return invalid_position;
}

usize StringView::find(UnicodeCodepoint codepoint_to_find) const
{
    usize offset = 0;
    while (offset < m_byte_count)
    {
        usize codepoint_width;
        UnicodeCodepoint codepoint = UTF8::bytes_to_codepoint(byte_span().slice(offset), codepoint_width);
        SE_ASSERT(codepoint != invalid_unicode_codepoint);

        if (codepoint == codepoint_to_find)
        {
            return offset;
        }
        offset += codepoint_width;
    }

    return invalid_position;
}

usize StringView::find_last(char ascii_character) const
{
    ReadonlyByteSpan span = byte_span();
    for (auto byte_it = span.rbegin(); byte_it != span.rend(); --byte_it)
    {
        if (*byte_it == ascii_character)
            return (byte_it - span.begin());
    }

    return invalid_size;
}

StringView StringView::slice(usize offset_in_bytes) const
{
    SE_ASSERT(offset_in_bytes <= m_byte_count);
    return StringView::unsafe_create_from_utf8(m_characters + offset_in_bytes, m_byte_count - offset_in_bytes);
}

StringView StringView::slice(usize offset_in_bytes, usize bytes_count) const
{
    SE_ASSERT(offset_in_bytes + bytes_count <= m_byte_count);
    return StringView::unsafe_create_from_utf8(m_characters + offset_in_bytes, bytes_count);
}

bool StringView::operator==(const StringView& other) const
{
    if (m_byte_count != other.m_byte_count)
    {
        return false;
    }

    for (usize byte_offset = 0; byte_offset < m_byte_count; ++byte_offset)
    {
        if (m_characters[byte_offset] != other.m_characters[byte_offset])
        {
            return false;
        }
    }

    return true;
}

bool StringView::operator!=(const StringView& other) const
{
    const bool are_equal = ((*this) == other);
    return !are_equal;
}

} // namespace SE
