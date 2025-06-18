// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/String.h>
#include <Runtime/Core/Memory/MemoryOperations.h>

namespace SE
{

String::String()
{
    m_ByteCount = 1;
    m_InlineBuffer[0] = 0;
}

String::~String()
{
    if (IsStoredOnHeap())
    {
        String::FreeMemory(m_HeapBuffer, m_ByteCount);
    }
}

String::String(const String& other)
    : m_ByteCount(other.m_ByteCount)
{
    if (IsStoredInline())
    {
        MemoryCopy(m_InlineBuffer, other.m_InlineBuffer, m_ByteCount);
    }
    else
    {
        m_HeapBuffer = String::AllocateMemory(m_ByteCount);
        MemoryCopy(m_HeapBuffer, other.m_HeapBuffer, m_ByteCount);
    }
}

String::String(String&& other) noexcept
    : m_ByteCount(other.m_ByteCount)
{
    if (IsStoredInline())
    {
        MemoryCopy(m_InlineBuffer, other.m_InlineBuffer, m_ByteCount);
        MemoryZero(other.m_InlineBuffer, INLINE_CAPACITY);
    }
    else
    {
        m_HeapBuffer = other.m_HeapBuffer;
        other.m_HeapBuffer = nullptr;
    }

    /* Invalidate the other string. */
    other.m_ByteCount = 1;
    other.m_InlineBuffer[0] = 0;
}

String::String(StringView view)
    : m_ByteCount(view.ByteCount() + 1)
{
    char* destination_buffer = m_InlineBuffer;

    if (IsStoredOnHeap())
    {
        m_HeapBuffer = String::AllocateMemory(m_ByteCount);
        destination_buffer = m_HeapBuffer;
    }

    MemoryCopy(destination_buffer, view.Characters(), view.ByteCount());
    destination_buffer[m_ByteCount - 1] = 0;
}

String& String::operator=(const String& other)
{
    if (this == &other)
    {
        /* Handle the self-assignment case. */
        return *this;
    }

    if (other.IsStoredInline())
    {
        if (IsStoredInline())
        {
            /* Discard the data previously stored by the string container. */
            MemoryZero(m_InlineBuffer, INLINE_CAPACITY);
        }
        else
        {
            String::FreeMemory(m_HeapBuffer, m_ByteCount);
            m_HeapBuffer = nullptr;
        }

        MemoryCopy(m_InlineBuffer, other.m_InlineBuffer, other.m_ByteCount);
    }
    else
    {
        if (IsStoredInline())
        {
            /* Discard the data previously stored by the string container. */
            MemoryZero(m_InlineBuffer, INLINE_CAPACITY);
            m_HeapBuffer = String::AllocateMemory(other.m_ByteCount);
        }
        else
        {
            if (m_ByteCount != other.m_ByteCount)
            {
                String::FreeMemory(m_HeapBuffer, m_ByteCount);
                m_HeapBuffer = String::AllocateMemory(other.m_ByteCount);
            }
        }

        MemoryCopy(m_HeapBuffer, other.m_HeapBuffer, other.m_ByteCount);
    }

    m_ByteCount = other.m_ByteCount;
    return *this;
}

String& String::operator=(String&& other) noexcept
{
    if (this == &other)
    {
        /* Handle the self-assignment case. */
        return *this;
    }

    if (IsStoredInline())
    {
        /* Discard the data previously stored by the string container. */
        MemoryZero(m_InlineBuffer, INLINE_CAPACITY);
    }
    else
    {
        String::FreeMemory(m_HeapBuffer, m_ByteCount);
    }

    m_ByteCount = other.m_ByteCount;

    if (IsStoredInline())
    {
        MemoryCopy(m_InlineBuffer, other.m_InlineBuffer, m_ByteCount);
        MemoryZero(other.m_InlineBuffer, INLINE_CAPACITY);
    }
    else
    {
        m_HeapBuffer = other.m_HeapBuffer;
        other.m_HeapBuffer = nullptr;
    }

    /* Invalidate the other string. */
    other.m_ByteCount = 1;
    other.m_InlineBuffer[0] = 0;

    return *this;
}

String& String::operator=(StringView view)
{
    if (view.ByteCount() + 1 <= INLINE_CAPACITY)
    {
        if (IsStoredInline())
        {
            /* Discard the data previously stored by the string container. */
            MemoryZero(m_InlineBuffer, INLINE_CAPACITY);
        }
        else
        {
            String::FreeMemory(m_HeapBuffer, m_ByteCount);
            m_HeapBuffer = nullptr;
        }

        MemoryCopy(m_InlineBuffer, view.Characters(), view.ByteCount());
        m_InlineBuffer[view.ByteCount()] = 0;
    }
    else
    {
        if (IsStoredInline())
        {
            /* Discard the data previously stored by the string container. */
            MemoryZero(m_InlineBuffer, INLINE_CAPACITY);
            m_HeapBuffer = String::AllocateMemory(view.ByteCount() + 1);
        }
        else
        {
            if (m_ByteCount != view.ByteCount() + 1)
            {
                String::FreeMemory(m_HeapBuffer, m_ByteCount);
                m_HeapBuffer = String::AllocateMemory(view.ByteCount() + 1);
            }
        }

        MemoryCopy(m_HeapBuffer, view.Characters(), view.ByteCount());
        m_HeapBuffer[view.ByteCount()] = 0;
    }

    m_ByteCount = view.ByteCount() + 1;
    return *this;
}

char* String::AllocateMemory(usize byteCount)
{
    /* The string container should never allocate any heap memory block that is smaller than
     * the inline buffer, as it would defeat the whole purpose of the small-string optimization. */
    SE_CHECK(byteCount > INLINE_CAPACITY);
    
    return static_cast<char*>(::operator new(byteCount * sizeof(char)));
}

void String::FreeMemory(char* heapBuffer, MAYBE_UNUSED usize byteCount)
{
    /* The string container should never allocate in the first place any heap memory block that is smaller
     * than the inline buffer, as it would defeat the whole purpose of the small-string optimization. */
    SE_CHECK(byteCount > INLINE_CAPACITY);

    ::operator delete(heapBuffer);
}

}
