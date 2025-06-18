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
    static constexpr usize INLINE_CAPACITY = sizeof(char*);
    static_assert(INLINE_CAPACITY > 0);

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
    NODISCARD FORCEINLINE bool IsStoredInline() const { return m_ByteCount <= INLINE_CAPACITY; }
    NODISCARD FORCEINLINE bool IsStoredOnHeap() const { return m_ByteCount > INLINE_CAPACITY; }

    NODISCARD FORCEINLINE const char* Characters() const { return IsStoredInline() ? m_InlineBuffer: m_HeapBuffer; }

    NODISCARD FORCEINLINE usize ByteCount() const
    {
        /* The string container should never be in the state where the internal character array
         * doesn't contain the null-termination byte. */
        SE_CHECK(m_ByteCount > 0);

        return m_ByteCount - sizeof(char);
    }

    NODISCARD FORCEINLINE usize ByteCountWithNullTerminator() const
    {
        /* The string container should never be in the state where the internal character array
         * doesn't contain the null-termination byte. */
        SE_CHECK(m_ByteCount > 0);

        return m_ByteCount;
    }

private:
    NODISCARD static char* AllocateMemory(usize byteCount);
    static void FreeMemory(char* heapBuffer, usize byteCount);

private:
    usize m_ByteCount;
    union
    {
        char* m_HeapBuffer;
        char m_InlineBuffer[INLINE_CAPACITY];
    };
};

}
