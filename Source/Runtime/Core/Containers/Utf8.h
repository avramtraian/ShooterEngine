/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/Span.h>

namespace SE
{

class UTF8
{
public:
    //
    // Converts a sequence of UTF-8 encoded bytes to the corresponding Unicode codepoint.
    // If the byte sequence is not valid UTF-8, 'invalid_unicode_codepoint' will be returned and
    // the 'out_codepoint_width' parameter will be set as zero.
    //
    NODISCARD SHOOTER_API static UnicodeCodepoint bytes_to_codepoint(ReadonlyByteSpan byte_span, usize& out_codepoint_width);

    //
    // Computes the width (in bytes) of the codepoint, encoded as UTF-8, that is represented
    // by the given UTF-8 encoded byte sequence.
    // If the byte sequence is not valid UTF-8, zero will be returned.
    //
    NODISCARD SHOOTER_API static usize bytes_to_codepoint_width(ReadonlyByteSpan byte_span);

    //
    // Converts a Unicode codepoint to a UTF-8 encoded byte sequence.
    // If the codepoint is not valid Unicode, no memory will be written and zero will be returned.
    // If the destination buffer is not big enough, no memory will be written and zero will be returned.
    //
    NODISCARD SHOOTER_API static usize bytes_from_codepoint(UnicodeCodepoint codepoint, WriteonlyByteSpan destination_byte_span);

    //
    // Computes the width (in bytes) of a Unicode codepoint, encoded as UTF-8.
    // If the codepoint is not valid Unicode, zero will be returned.
    //
    NODISCARD SHOOTER_API static usize codepoint_width(UnicodeCodepoint codepoint);

public:
    //
    // Computes the number of codepoints that the UTF-8 encoded byte sequence contains.
    // The length will include any character and the function will not stop if it encounters a null-termination
    // character. If the byte sequence is not valid UTF-8, 'invalid_size' will be returned.
    //
    NODISCARD SHOOTER_API static usize length(ReadonlyByteSpan byte_span);

    //
    // Determines the number of bytes that a null-terminated UTF-8 string occupies.
    // If 'bytes' is nullptr, zero will be returned.
    // If the provided byte sequence is not valid UTF-8 'invalid_size' will be returned.
    //
    NODISCARD SHOOTER_API static usize byte_count(ReadonlyBytes bytes);

    NODISCARD SHOOTER_API static bool check_validity(ReadonlyByteSpan byte_span);
};

} // namespace SE
