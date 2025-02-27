// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/StringView.h>
#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class String
{
public:
    static constexpr usize inline_capacity = sizeof(char*);
    static_assert(inline_capacity > 0);

public:
    SHOOTER_API String();
    SHOOTER_API ~String();

    SHOOTER_API String(const String& other);
    SHOOTER_API String(String&& other) noexcept;
    SHOOTER_API String(StringView view);

    SHOOTER_API String& operator=(const String& other);
    SHOOTER_API String& operator=(String&& other) noexcept;
    SHOOTER_API String& operator=(StringView view);

public:
    NODISCARD FORCEINLINE bool is_stored_inline() const { return m_byte_count <= inline_capacity; }
    NODISCARD FORCEINLINE bool is_stored_on_heap() const { return m_byte_count > inline_capacity; }

    NODISCARD FORCEINLINE const char* characters() const { return is_stored_inline() ? m_inline_buffer : m_heap_buffer; }

    NODISCARD FORCEINLINE usize byte_count() const
    {
        /* The string container should never be in the state where the internal character array
         * doesn't contain the null-termination byte. */
        SE_CHECK(m_byte_count > 0);

        return m_byte_count - sizeof(char);
    }

    NODISCARD FORCEINLINE usize byte_count_with_null_terminator() const
    {
        /* The string container should never be in the state where the internal character array
         * doesn't contain the null-termination byte. */
        SE_CHECK(m_byte_count > 0);

        return m_byte_count;
    }

private:
    NODISCARD static char* allocate_memory(usize in_byte_count);
    static void free_memory(char* in_heap_buffer, usize in_byte_count);

private:
    usize m_byte_count;
    union
    {
        char* m_heap_buffer;
        char m_inline_buffer[inline_capacity];
    };
};

}
