// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

#define VIEW(stringLiteral) ::SE::StringView::FromUTF8(stringLiteral, sizeof(stringLiteral) - 1)

namespace SE
{

NODISCARD RUNTIME_API uint64 GetCityHash64(const char* characters, usize byteCount);

class StringView
{
public:
    NODISCARD ALWAYS_INLINE static uint64 GetHash(const StringView& view)
    {
        const uint64 hashValue = GetCityHash64(view.Characters(), view.ByteCount());
        return hashValue;
    }

public:
    NODISCARD RUNTIME_API static StringView FromUTF8(const char* characters, usize byteCount);
    NODISCARD RUNTIME_API static StringView FromUTF8(const char* nullTerminatedCharacters);

public:
    ALWAYS_INLINE constexpr StringView()
        : m_ByteCount(0)
        , m_Characters(nullptr)
    {}

    ALWAYS_INLINE constexpr StringView(const StringView& other)
        : m_ByteCount(other.m_ByteCount)
        , m_Characters(other.m_Characters)
    {}

    ALWAYS_INLINE constexpr StringView(StringView&& other) noexcept
        : m_ByteCount(other.m_ByteCount)
        , m_Characters(other.m_Characters)
    {
        other.m_Characters = nullptr;
        other.m_ByteCount = 0;
    }

    ALWAYS_INLINE constexpr StringView& operator=(const StringView& other)
    {
        m_Characters = other.m_Characters;
        m_ByteCount = other.m_ByteCount;
        return *this;
    }

    ALWAYS_INLINE constexpr StringView& operator=(StringView&& other) noexcept
    {
        m_Characters = other.m_Characters;
        m_ByteCount = other.m_ByteCount;
        other.m_Characters = nullptr;
        other.m_ByteCount = 0;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE usize ByteCount() const { return m_ByteCount; }
    NODISCARD ALWAYS_INLINE const char* Characters() const { return m_Characters; }

    NODISCARD ALWAYS_INLINE bool IsEmpty() const { return m_ByteCount == 0; }
    NODISCARD ALWAYS_INLINE bool HasCharacters() const { return m_ByteCount > 0; }

public:
    NODISCARD ALWAYS_INLINE bool operator==(const StringView& other) const
    {
        if (m_ByteCount != other.m_ByteCount)
            return false;

        for (usize byteOffset = 0; byteOffset < m_ByteCount; ++byteOffset)
        {
            if (m_Characters[byteOffset] != other.m_Characters[byteOffset])
                return false;
        }

        return true;
    }

    NODISCARD ALWAYS_INLINE bool operator!=(const StringView& other) const
    {
        const bool areEqual = ((*this) == other);
        return !areEqual;
    }

private:
    usize       m_ByteCount;
    const char* m_Characters;
};

}
