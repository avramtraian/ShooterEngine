/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/Span.h>
#include <Core/Containers/StringView.h>

#define SE_FILEPATH_DELIMITATOR           '/'
#define SE_FILEPATH_EXTENSION_DELIMITATOR '.'

namespace SE
{

//
// Container that stores a UTF-8 encoded, null-terminated string.
// If the string is small enough, the threshold being determined by the platform
// architecture, no memory will be allocated from the heap and the characters will
// be instead stored inline. This allows small strings to be very efficient in terms
// of performance, as copying and creating them would be very cheap.
//
class String
{
    friend class StringBuilder;

public:
    static constexpr usize inline_capacity = sizeof(char*);
    static_assert(inline_capacity > 0);

public:
    SHOOTER_API static String create_from_utf8(const char* characters, usize byte_count);
    SHOOTER_API static String create_from_utf8(const char* null_terminated_characters);

public:
    SHOOTER_API String();
    SHOOTER_API ~String();

    SHOOTER_API String(const String& other);
    SHOOTER_API String(String&& other) noexcept;
    SHOOTER_API String(StringView string_view);

    SHOOTER_API String& operator=(const String& other);
    SHOOTER_API String& operator=(String&& other) noexcept;
    SHOOTER_API String& operator=(StringView string_view);

public:
    NODISCARD ALWAYS_INLINE StringView view() const
    {
        auto span = byte_span().as<const char>();
        // NOTE: We can be pretty sure that the held string is valid UTF-8, so checking
        //       for validity again would be a bit wasteful.
        return StringView::unsafe_create_from_utf8(span.elements(), span.count() * span.element_size());
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span()
    {
        SE_ASSERT(m_byte_count > 0);
        auto* bytes = reinterpret_cast<ReadWriteBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count - 1);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span() const
    {
        SE_ASSERT(m_byte_count > 0);
        const auto* bytes = reinterpret_cast<ReadonlyBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count - 1);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_byte_span()
    {
        SE_ASSERT(m_byte_count > 0);
        const auto* bytes = reinterpret_cast<ReadonlyBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count - 1);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span_with_null_termination()
    {
        SE_ASSERT(m_byte_count > 0);
        auto* bytes = reinterpret_cast<ReadWriteBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span_with_null_termination() const
    {
        SE_ASSERT(m_byte_count > 0);
        const auto* bytes = reinterpret_cast<ReadonlyBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_byte_span_with_null_termination()
    {
        SE_ASSERT(m_byte_count > 0);
        const auto* bytes = reinterpret_cast<ReadonlyBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count);
    }

    // This function is not recommended for general use. It is only meant to be used for communication with C APIs.
    NODISCARD ALWAYS_INLINE const char* c_str() const { return byte_span_with_null_termination().as<const char>().elements(); }

    NODISCARD ALWAYS_INLINE bool is_stored_inline() const { return m_byte_count <= inline_capacity; }
    NODISCARD ALWAYS_INLINE bool is_stored_on_heap() const { return m_byte_count > inline_capacity; }

public:
    NODISCARD SHOOTER_API String& append(StringView view_to_append);
    NODISCARD SHOOTER_API String operator+(StringView view_to_append) const;

    NODISCARD ALWAYS_INLINE String& append(const String& string_to_append) { return append(string_to_append.view()); }
    NODISCARD ALWAYS_INLINE String operator+(const String& string_to_append) const { return (*this) + string_to_append.view(); }

    SHOOTER_API void clear();

public: // Filepath API
    NODISCARD SHOOTER_API StringView path_parent() const;
    NODISCARD SHOOTER_API StringView path_filename() const;
    NODISCARD SHOOTER_API StringView path_stem() const;
    NODISCARD SHOOTER_API StringView path_extension() const;

public:
    NODISCARD ALWAYS_INLINE bool operator==(const String& other) const { return (view() == other.view()); }
    NODISCARD ALWAYS_INLINE bool operator==(StringView string_view) const { return (view() == string_view); }

    NODISCARD ALWAYS_INLINE bool operator!=(const String& other) const { return (view() != other.view()); }
    NODISCARD ALWAYS_INLINE bool operator!=(StringView string_view) const { return (view() != string_view); }

private:
    NODISCARD static char* allocate_memory(usize byte_count);
    static void release_memory(char* heap_buffer, usize byte_count);

private:
    union
    {
        char m_inline_buffer[inline_capacity];
        char* m_heap_buffer;
    };
    usize m_byte_count;
};

} // namespace SE
