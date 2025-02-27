// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/String.h>
#include <Runtime/Core/Memory/MemoryOperations.h>

namespace SE
{

String::String()
{
    m_byte_count = 1;
    m_inline_buffer[0] = 0;
}

String::~String()
{
    if (is_stored_on_heap())
    {
        String::free_memory(m_heap_buffer, m_byte_count);
    }
}

String::String(const String& other)
    : m_byte_count(other.m_byte_count)
{
    if (is_stored_inline())
    {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
    }
    else
    {
        m_heap_buffer = String::allocate_memory(m_byte_count);
        copy_memory(m_heap_buffer, other.m_heap_buffer, m_byte_count);
    }
}

String::String(String&& other) noexcept
    : m_byte_count(other.m_byte_count)
{
    if (is_stored_inline())
    {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
        zero_memory(other.m_inline_buffer, inline_capacity);
    }
    else
    {
        m_heap_buffer = other.m_heap_buffer;
        other.m_heap_buffer = nullptr;
    }

    /* Invalidate the other string. */
    other.m_byte_count = 1;
    other.m_inline_buffer[0] = 0;
}

String::String(StringView view)
    : m_byte_count(view.byte_count() + 1)
{
    char* destination_buffer = m_inline_buffer;

    if (is_stored_on_heap())
    {
        m_heap_buffer = String::allocate_memory(m_byte_count);
        destination_buffer = m_heap_buffer;
    }

    copy_memory(destination_buffer, view.characters(), view.byte_count());
    destination_buffer[m_byte_count - 1] = 0;
}

String& String::operator=(const String& other)
{
    if (this == &other)
    {
        /* Handle the self-assignment case. */
        return *this;
    }

    if (other.is_stored_inline())
    {
        if (is_stored_inline())
        {
            /* Discard the data previously stored by the string container. */
            zero_memory(m_inline_buffer, inline_capacity);
        }
        else
        {
            String::free_memory(m_heap_buffer, m_byte_count);
            m_heap_buffer = nullptr;
        }

        copy_memory(m_inline_buffer, other.m_inline_buffer, other.m_byte_count);
    }
    else
    {
        if (is_stored_inline())
        {
            /* Discard the data previously stored by the string container. */
            zero_memory(m_inline_buffer, inline_capacity);
            m_heap_buffer = String::allocate_memory(other.m_byte_count);
        }
        else
        {
            if (m_byte_count != other.m_byte_count)
            {
                String::free_memory(m_heap_buffer, m_byte_count);
                m_heap_buffer = String::allocate_memory(other.m_byte_count);
            }
        }

        copy_memory(m_heap_buffer, other.m_heap_buffer, other.m_byte_count);
    }

    m_byte_count = other.m_byte_count;
    return *this;
}

String& String::operator=(String&& other) noexcept
{
    if (this == &other)
    {
        /* Handle the self-assignment case. */
        return *this;
    }

    if (is_stored_inline())
    {
        /* Discard the data previously stored by the string container. */
        zero_memory(m_inline_buffer, inline_capacity);
    }
    else
    {
        String::free_memory(m_heap_buffer, m_byte_count);
    }

    m_byte_count = other.m_byte_count;

    if (is_stored_inline())
    {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
        zero_memory(other.m_inline_buffer, inline_capacity);
    }
    else
    {
        m_heap_buffer = other.m_heap_buffer;
        other.m_heap_buffer = nullptr;
    }

    /* Invalidate the other string. */
    other.m_byte_count = 1;
    other.m_inline_buffer[0] = 0;

    return *this;
}

String& String::operator=(StringView view)
{
    if (view.byte_count() + 1 <= inline_capacity)
    {
        if (is_stored_inline())
        {
            /* Discard the data previously stored by the string container. */
            zero_memory(m_inline_buffer, inline_capacity);
        }
        else
        {
            String::free_memory(m_heap_buffer, m_byte_count);
            m_heap_buffer = nullptr;
        }

        copy_memory(m_inline_buffer, view.characters(), view.byte_count());
        m_inline_buffer[view.byte_count()] = 0;
    }
    else
    {
        if (is_stored_inline())
        {
            /* Discard the data previously stored by the string container. */
            zero_memory(m_inline_buffer, inline_capacity);
            m_heap_buffer = String::allocate_memory(view.byte_count() + 1);
        }
        else
        {
            if (m_byte_count != view.byte_count() + 1)
            {
                String::free_memory(m_heap_buffer, m_byte_count);
                m_heap_buffer = String::allocate_memory(view.byte_count() + 1);
            }
        }

        copy_memory(m_heap_buffer, view.characters(), view.byte_count());
        m_heap_buffer[view.byte_count()] = 0;
    }

    m_byte_count = view.byte_count() + 1;
    return *this;
}

char* String::allocate_memory(usize in_byte_count)
{
    /* The string container should never allocate any heap memory block that is smaller than
     * the inline buffer, as it would defeat the whole purpose of the small-string optimization. */
    SE_CHECK(in_byte_count > inline_capacity);
    
    return static_cast<char*>(::operator new(in_byte_count * sizeof(char)));
}

void String::free_memory(char* in_heap_buffer, MAYBE_UNUSED usize in_byte_count)
{
    /* The string container should never allocate in the first place any heap memory block that is smaller
     * than the inline buffer, as it would defeat the whole purpose of the small-string optimization. */
    SE_CHECK(in_byte_count > inline_capacity);

    ::operator delete(in_heap_buffer);
}

}
