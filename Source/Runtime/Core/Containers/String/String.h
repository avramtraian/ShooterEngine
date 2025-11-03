// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/Allocator.h>
#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/Core/Containers/String/StringView.h>
#include <Runtime/Core/Memory/MemoryOperations.h>

namespace SE
{

NODISCARD SHOOTER_API uint64 GetCityHash64(const char* characters, usize byteCount);

template<typename Allocator>
class TString
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    static constexpr usize INLINE_CAPACITY = sizeof(void*);
    static_assert(INLINE_CAPACITY > 0);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct HeapData
    {
#if SE_COMPILER_CLANG
    #pragma clang diagnostic ignored "-Wc99-extensions"
#endif // SE_COMPILER_CLANG

        uint32 ReferenceCount;
        char   Characters[];
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD FORCEINLINE static uint64 GetHash(const TString& string)
    {
        const uint64 hashValue = GetCityHash64(string.Characters(), string.ByteCountWithoutNullTerminator());
        return hashValue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE TString()
    {
        m_ByteCount           = 1;
        m_InlineCharacters[0] = '\0';
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE ~TString() { Clear(); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE TString(const TString& other)
        : m_ByteCount(other.m_ByteCount)
    {
        if (other.IsStoredInline())
        {
            MemoryCopy(m_InlineCharacters, other.m_InlineCharacters, m_ByteCount);
        }
        else
        {
            m_HeapData = other.m_HeapData;
            m_HeapData->ReferenceCount++;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE TString(TString&& other) noexcept
        : m_ByteCount(other.m_ByteCount)
    {
        if (other.IsStoredInline())
        {
            MemoryCopy(m_InlineCharacters, other.m_InlineCharacters, m_ByteCount);
            MemoryZero(other.m_InlineCharacters, INLINE_CAPACITY);
        }
        else
        {
            m_HeapData       = other.m_HeapData;
            other.m_HeapData = nullptr;
        }

        other.m_ByteCount           = 1;
        other.m_InlineCharacters[0] = '\0';
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE TString(StringView view)
        : m_ByteCount(view.ByteCount() + sizeof(char))
    {
        char* destinationCharacters = m_InlineCharacters;
        if (IsStoredOnHeap())
        {
            m_HeapData                 = TString::AllocateHeapData(m_ByteCount);
            m_HeapData->ReferenceCount = 1;
            destinationCharacters      = m_HeapData->Characters;
        }

        MemoryCopy(destinationCharacters, view.Characters(), view.ByteCount());
        destinationCharacters[view.ByteCount()] = '\0';
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE TString& operator=(const TString& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Clear();
        m_ByteCount = other.m_ByteCount;

        if (other.IsStoredInline())
        {
            MemoryCopy(m_InlineCharacters, other.m_InlineCharacters, other.m_ByteCount);
        }
        else
        {
            m_HeapData = other.m_HeapData;
            m_HeapData->ReferenceCount++;
        }

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE TString& operator=(TString&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Clear();
        m_ByteCount = other.m_ByteCount;

        if (other.IsStoredInline())
        {
            MemoryCopy(m_InlineCharacters, other.m_InlineCharacters, m_ByteCount);
            MemoryZero(other.m_InlineCharacters, INLINE_CAPACITY);
        }
        else
        {
            m_HeapData       = other.m_HeapData;
            other.m_HeapData = nullptr;
        }

        other.m_ByteCount           = 1;
        other.m_InlineCharacters[0] = '\0';

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE TString& operator=(StringView view)
    {
        Clear();
        m_ByteCount                 = view.ByteCount() + sizeof(char);

        char* destinationCharacters = m_InlineCharacters;
        if (IsStoredOnHeap())
        {
            m_HeapData                 = TString::AllocateHeapData(m_ByteCount);
            m_HeapData->ReferenceCount = 1;
            destinationCharacters      = m_HeapData->Characters;
        }

        MemoryCopy(destinationCharacters, view.Characters(), view.ByteCount());
        destinationCharacters[view.ByteCount()] = '\0';

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD FORCEINLINE usize ByteCountWithNullTerminator() const
    {
        SE_ASSERT(m_ByteCount > 0);
        return m_ByteCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE usize ByteCountWithoutNullTerminator() const
    {
        SE_ASSERT(m_ByteCount > 0);
        return m_ByteCount - 1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE bool IsEmpty() const { return (ByteCountWithoutNullTerminator() == 0); }
    NODISCARD FORCEINLINE bool HasCharacters() const { return (ByteCountWithoutNullTerminator() > 0); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE const char* Characters() const
    {
        if (IsStoredInline())
            return m_InlineCharacters;
        return m_HeapData->Characters;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE char* NonConstCharacters() const
    {
        const char* constCharacters = Characters();
        return const_cast<char*>(constCharacters);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE void Clear()
    {
        if (IsStoredInline())
        {
            MemoryZero(m_InlineCharacters, INLINE_CAPACITY);
        }
        else
        {
            m_HeapData->ReferenceCount--;
            if (m_HeapData->ReferenceCount == 0)
                ReleaseHeapData(m_HeapData, m_ByteCount);
            m_HeapData = nullptr;
        }

        m_ByteCount           = 1;
        m_InlineCharacters[0] = '\0';
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    template<typename OtherAllocator>
    NODISCARD FORCEINLINE bool operator==(const TString<OtherAllocator>& other) const
    {
        if (m_ByteCount != other.m_ByteCount)
            return false;

        const char* thisCharacters  = Characters();
        const char* otherCharacters = other.Characters();

        for (usize byteOffset = 0; byteOffset < m_ByteCount; ++byteOffset)
        {
            if (thisCharacters[byteOffset] != otherCharacters[byteOffset])
                return false;
        }

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE bool operator==(StringView view) const
    {
        if (ByteCountWithoutNullTerminator() != view.ByteCount())
            return false;

        const char* thisCharacters  = Characters();
        const char* otherCharacters = view.Characters();

        for (usize byteOffset = 0; byteOffset < view.ByteCount(); ++byteOffset)
        {
            if (thisCharacters[byteOffset] != otherCharacters[byteOffset])
                return false;
        }

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename OtherAllocator>
    NODISCARD FORCEINLINE bool operator!=(const TString<OtherAllocator>& other) const
    {
        const bool areEqual = ((*this) == other);
        return !areEqual;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE bool operator!=(StringView view) const
    {
        const bool areEqual = ((*this) == view);
        return !areEqual;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD FORCEINLINE Optional<usize> Find(StringView view, usize searchStartByteOffset = 0) const
    {
        if (view.ByteCount() > ByteCountWithoutNullTerminator())
        {
            return {};
        }

        const char* thisCharacters = Characters();
        for (usize byteOffset = searchStartByteOffset; byteOffset <= ByteCountWithoutNullTerminator() - view.ByteCount(); ++byteOffset)
        {
            bool areEqual = true;
            for (usize viewByteOffset = 0; viewByteOffset < view.ByteCount(); ++viewByteOffset)
            {
                if (thisCharacters[byteOffset + viewByteOffset] != view.Characters()[viewByteOffset])
                {
                    areEqual = false;
                    break;
                }
            }

            if (areEqual)
            {
                return byteOffset;
            }
        }

        return {};
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE bool Contains(StringView view, usize searchStartByteOffset = 0) const
    {
        Optional<usize> substringByteOffset = Find(view, searchStartByteOffset);
        return substringByteOffset.HasValue();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    NODISCARD FORCEINLINE static HeapData* AllocateHeapData(usize byteCount)
    {
        const usize allocationSize = sizeof(HeapData) + byteCount;
        void*       memoryBlock    = Allocator::Allocate(allocationSize);
        HeapData*   heapData       = static_cast<HeapData*>(memoryBlock);
        heapData->ReferenceCount   = 0;
        return heapData;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE static void ReleaseHeapData(HeapData* heapData, usize byteCount)
    {
        MAYBE_UNUSED const usize allocationSize = sizeof(HeapData) + byteCount;
        Allocator::Release(heapData);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    NODISCARD FORCEINLINE bool IsStoredInline() const { return (m_ByteCount <= INLINE_CAPACITY); }
    NODISCARD FORCEINLINE bool IsStoredOnHeap() const { return (m_ByteCount > INLINE_CAPACITY); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    usize m_ByteCount;
    union
    {
        char      m_InlineCharacters[INLINE_CAPACITY];
        HeapData* m_HeapData;
    };
};

using String = TString<DefaultAllocator>;

} // namespace SE
