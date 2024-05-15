/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/Containers/String.h"
#include "Core/Memory/MemoryOperations.h"

namespace SE {

String String::create_from_utf8(const char* characters, usize byte_count)
{
    StringView view = StringView::create_from_utf8(characters, byte_count);
    return String(view);
}

String String::create_from_utf8(const char* null_terminated_characters)
{
    StringView view = StringView::create_from_utf8(null_terminated_characters);
    return String(view);
}

String::String()
{
    m_inline_buffer[0] = 0;
    m_byte_count = 1;
}

String::~String()
{
    if (is_stored_on_heap())
        release_memory(m_heap_buffer, m_byte_count);
}

String::String(const String& other)
{
    // Forward the constructor to the StringView overload.
    new (this) String(other.view());
}

String::String(String&& other) noexcept
    : m_heap_buffer(other.m_heap_buffer)
    , m_byte_count(other.m_byte_count)
{
    other.m_inline_buffer[0] = 0;
    other.m_byte_count = 1;
}

String::String(StringView string_view)
    : m_byte_count(string_view.byte_span().count() + 1)
{
    char* destination_buffer = m_inline_buffer;

    if (is_stored_on_heap())
    {
        m_heap_buffer = allocate_memory(m_byte_count);
        destination_buffer = m_heap_buffer;
    }

    copy_memory_from_span(destination_buffer, string_view.byte_span());
    destination_buffer[m_byte_count - 1] = 0;
}

String& String::append(StringView view_to_append)
{
    if (view_to_append.is_empty())
        return *this;

    const usize new_byte_count = m_byte_count + view_to_append.byte_span().count();

    if (new_byte_count <= inline_capacity)
    {
        SE_ASSERT_DEBUG(m_byte_count > 0);
        copy_memory_from_span(m_inline_buffer + m_byte_count - 1, view_to_append.byte_span());
        m_byte_count = new_byte_count;
        m_inline_buffer[m_byte_count - 1] = 0;
        return *this;
    }

    char* new_heap_buffer = allocate_memory(new_byte_count);
    copy_memory_from_span(new_heap_buffer, byte_span());
    SE_ASSERT_DEBUG(m_byte_count > 0);
    copy_memory_from_span(new_heap_buffer + m_byte_count - 1, view_to_append.byte_span());
    new_heap_buffer[new_byte_count - 1] = 0;

    if (is_stored_on_heap())
        release_memory(m_heap_buffer, m_byte_count);

    m_heap_buffer = new_heap_buffer;
    m_byte_count = new_byte_count;
    return *this;
}

String String::operator+(StringView view_to_append) const
{
    const usize byte_count = m_byte_count + view_to_append.byte_span().count();

    String result;
    char* destination_buffer = result.m_inline_buffer;

    if (byte_count > inline_capacity)
    {
        result.m_heap_buffer = allocate_memory(byte_count);
        destination_buffer = result.m_heap_buffer;
    }

    copy_memory_from_span(destination_buffer, byte_span());
    SE_ASSERT_DEBUG(m_byte_count > 0);
    copy_memory_from_span(destination_buffer + m_byte_count - 1, view_to_append.byte_span());
    destination_buffer[byte_count - 1] = 0;

    result.m_byte_count = byte_count;
    return result;
}

StringView String::path_parent() const
{
    const StringView this_as_view = view();

    const usize delimitator_position = this_as_view.find_last(SE_FILEPATH_DELIMITATOR);
    if (delimitator_position == invalid_size)
        return {};

    return this_as_view.slice(0, delimitator_position);
}

StringView String::path_filename() const
{
    const StringView this_as_view = view();

    const usize delimitator_position = this_as_view.find_last(SE_FILEPATH_DELIMITATOR);
    if (delimitator_position == invalid_size)
        return this_as_view;

    return this_as_view.slice(delimitator_position + 1);
}

StringView String::path_stem() const
{
    const StringView filename = path_filename();

    const usize extension_delimitator_position = filename.find(SE_FILEPATH_EXTENSION_DELIMITATOR);
    if (extension_delimitator_position == invalid_size)
        return filename;

    return filename.slice(0, extension_delimitator_position);
}

StringView String::path_extension() const
{
    const StringView filename = path_filename();

    const usize extension_delimitator_position = filename.find(SE_FILEPATH_EXTENSION_DELIMITATOR);
    if (extension_delimitator_position == invalid_size)
        return {};

    return filename.slice(extension_delimitator_position + 1);
}

String& String::operator=(const String& other)
{
    *this = other.view();
    return *this;
}

String& String::operator=(String&& other) noexcept
{
    if (is_stored_on_heap())
        release_memory(m_heap_buffer, m_byte_count);

    m_heap_buffer = other.m_heap_buffer;
    m_byte_count = other.m_byte_count;

    zero_memory(other.m_inline_buffer, inline_capacity);
    other.m_inline_buffer[0] = 0;
    other.m_byte_count = 1;

    return *this;
}

String& String::operator=(StringView string_view)
{
    usize byte_count = string_view.byte_span().count() + 1;
    char* destination_buffer = m_inline_buffer;

    if (is_stored_inline())
    {
        if (byte_count > inline_capacity)
        {
            m_heap_buffer = allocate_memory(byte_count);
            destination_buffer = m_heap_buffer;
        }
    }
    else
    {
        if (byte_count <= inline_capacity)
        {
            release_memory(m_heap_buffer, m_byte_count);
        }
        else
        {
            if (m_byte_count != byte_count)
            {
                release_memory(m_heap_buffer, m_byte_count);
                m_heap_buffer = allocate_memory(byte_count);
            }
            destination_buffer = m_heap_buffer;
        }
    }

    m_byte_count = byte_count;
    copy_memory_from_span(destination_buffer, string_view.byte_span());
    destination_buffer[m_byte_count - 1] = 0;

    return *this;
}

char* String::allocate_memory(usize byte_count)
{
    void* memory_block = ::operator new(byte_count);
    SE_ASSERT(memory_block);
    return reinterpret_cast<char*>(memory_block);
}

void String::release_memory(char* heap_buffer, usize byte_count)
{
    // NOTE: The standard operator delete doesn't need the capacity of the memory block.
    //       However, in future implementations we might switch to a custom memory allocator,
    //       so having this crucial information available out-of-the-box is really handy.
    (void)byte_count;
    ::operator delete(heap_buffer);
}

} // namespace SE