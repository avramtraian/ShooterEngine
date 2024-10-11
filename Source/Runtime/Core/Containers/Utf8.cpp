/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/Utf8.h>

namespace SE
{

UnicodeCodepoint UTF8::bytes_to_codepoint(ReadonlyByteSpan byte_span, usize& out_codepoint_width)
{
    if (byte_span.count() == 0)
    {
        out_codepoint_width = 0;
        return invalid_unicode_codepoint;
    }

    if ((byte_span.elements()[0] & 0x80) == 0x00)
    {
        out_codepoint_width = 1;
        return (UnicodeCodepoint)byte_span.elements()[0];
    }

    if ((byte_span.elements()[0] & 0xE0) == 0xC0)
    {
        if (byte_span.count() < 2)
        {
            out_codepoint_width = 0;
            return invalid_unicode_codepoint;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (byte_span.elements()[0] & 0x1F) << 6;
        codepoint += (byte_span.elements()[1] & 0x3F) << 0;

        out_codepoint_width = 2;
        return codepoint;
    }

    if ((byte_span.elements()[0] & 0xF0) == 0xE0)
    {
        if (byte_span.count() < 3)
        {
            out_codepoint_width = 0;
            return invalid_unicode_codepoint;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (byte_span.elements()[0] & 0x1F) << 12;
        codepoint += (byte_span.elements()[1] & 0x3F) << 6;
        codepoint += (byte_span.elements()[2] & 0x3F) << 0;

        out_codepoint_width = 3;
        return codepoint;
    }

    if ((byte_span.elements()[0] & 0xF8) == 0xF0)
    {
        if (byte_span.count() < 4)
        {
            out_codepoint_width = 0;
            return invalid_unicode_codepoint;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (byte_span.elements()[0] & 0x1F) << 18;
        codepoint += (byte_span.elements()[1] & 0x3F) << 12;
        codepoint += (byte_span.elements()[2] & 0x3F) << 6;
        codepoint += (byte_span.elements()[3] & 0x3F) << 0;

        out_codepoint_width = 4;
        return codepoint;
    }

    out_codepoint_width = 0;
    return invalid_unicode_codepoint;
}

usize UTF8::bytes_to_codepoint_width(ReadonlyByteSpan byte_span)
{
    if (byte_span.count() == 0)
        return 0;

    if ((byte_span.elements()[0] & 0x80) == 0x00)
        return 1;

    if ((byte_span.elements()[0] & 0xE0) == 0xC0)
    {
        if (byte_span.count() < 2)
            return 0;
        return 2;
    }

    if ((byte_span.elements()[0] & 0xF0) == 0xE0)
    {
        if (byte_span.count() < 3)
            return 0;
        return 3;
    }

    if ((byte_span.elements()[0] & 0xF8) == 0xF0)
    {
        if (byte_span.count() < 4)
            return 0;
        return 4;
    }

    return 0;
}

usize UTF8::bytes_from_codepoint(UnicodeCodepoint codepoint, WriteonlyByteSpan destination_byte_span)
{
    if (codepoint <= 0x007F)
    {
        if (destination_byte_span.count() < 1)
            return 0;
        destination_byte_span.elements()[0] = (ReadWriteByte)codepoint;
        return 1;
    }

    if (0x0080 <= codepoint && codepoint <= 0x07FF)
    {
        if (destination_byte_span.count() < 2)
            return 0;
        destination_byte_span.elements()[0] = ((codepoint >> 6) & 0x1F) | 0xC0;
        destination_byte_span.elements()[1] = ((codepoint >> 0) & 0x3F) | 0x80;
        return 2;
    }

    if (0x0800 <= codepoint && codepoint <= 0xFFFF)
    {
        if (destination_byte_span.count() < 3)
            return 0;
        destination_byte_span.elements()[0] = ((codepoint >> 12) & 0x0F) | 0xE0;
        destination_byte_span.elements()[1] = ((codepoint >> 6) & 0x3F) | 0x80;
        destination_byte_span.elements()[2] = ((codepoint >> 0) & 0x3F) | 0x80;
        return 3;
    }

    if (0x10000 <= codepoint)
    {
        if (destination_byte_span.count() < 4)
            return 0;
        destination_byte_span.elements()[0] = ((codepoint >> 18) & 0x07) | 0xF0;
        destination_byte_span.elements()[1] = ((codepoint >> 12) & 0x3F) | 0x80;
        destination_byte_span.elements()[2] = ((codepoint >> 6) & 0x3F) | 0x80;
        destination_byte_span.elements()[3] = ((codepoint >> 0) & 0x3F) | 0x80;
        return 4;
    }

    return 0;
}

usize UTF8::codepoint_width(UnicodeCodepoint codepoint)
{
    if (codepoint <= 0x007F)
        return 1;
    if (0x0080 <= codepoint && codepoint <= 0x07FF)
        return 2;
    if (0x0800 <= codepoint && codepoint <= 0xFFFF)
        return 3;
    if (0x10000 <= codepoint)
        return 4;

    return 0;
}

usize UTF8::length(ReadonlyByteSpan byte_span)
{
    usize len = 0;
    usize byte_offset = 0;

    while (byte_offset < byte_span.count())
    {
        usize codepoint_width = bytes_to_codepoint_width(byte_span.slice(byte_offset));
        if (codepoint_width == 0)
            return invalid_size;

        ++len;
        byte_offset += codepoint_width;
    }

    return len;
}

usize UTF8::byte_count(ReadonlyBytes bytes)
{
    if (!bytes)
    {
        // If the 'bytes' pointer is null, return zero - the string is empty.
        return 0;
    }

    ReadonlyBytes base = bytes;
    while (*bytes++)
    {}

    // Determining the number of bytes this way doesn't guarantee that the byte sequence
    // is valid UTF-8, so a validation must now be performed.
    const usize byte_count = bytes - base;

    // Check that the byte sequence is actually valid UTF-8.
    if (!check_validity({ base, byte_count }))
    {
        // If not, return 'invalid_size' in order to signal an error.
        return invalid_size;
    }

    return byte_count;
}

bool UTF8::check_validity(ReadonlyByteSpan byte_span)
{
    usize byte_offset = 0;
    while (byte_offset < byte_span.count())
    {
        const usize codepoint_width = bytes_to_codepoint_width(byte_span.slice(byte_offset));
        if (codepoint_width == 0)
        {
            // A valid UTF-8 codepoint can't have a width of zero, so the string is not correctly encoded.
            return false;
        }
        byte_offset += codepoint_width;
    }
    return true;
}

} // namespace SE
