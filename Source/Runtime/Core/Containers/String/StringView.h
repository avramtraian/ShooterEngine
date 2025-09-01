// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

#define VIEW(stringLiteral) ::SE::StringView::FromUTF8(stringLiteral, sizeof(stringLiteral) - 1)

namespace SE
{

NODISCARD SHOOTER_API uint64 GetCityHash64(const char* characters, usize byteCount);

class StringView
{
public:
    NODISCARD FORCEINLINE static uint64 GetHash(const StringView& view)
    {
        const uint64 hashValue = GetCityHash64(view.Characters(), view.ByteCount());
        return hashValue;
    }

public:
    NODISCARD SHOOTER_API static StringView FromUTF8(const char* characters, usize byteCount);
    NODISCARD SHOOTER_API static StringView FromUTF8(const char* nullTerminatedCharacters);

public:
    FORCEINLINE constexpr StringView()
        : m_ByteCount(0)
        , m_Characters(nullptr)
    {}

    FORCEINLINE constexpr StringView(const StringView& other)
        : m_Characters(other.m_Characters)
        , m_ByteCount(other.m_ByteCount)
    {}

    FORCEINLINE constexpr StringView(StringView&& other) noexcept
        : m_Characters(other.m_Characters)
        , m_ByteCount(other.m_ByteCount)
    {
        other.m_Characters = nullptr;
        other.m_ByteCount = 0;
    }

    FORCEINLINE constexpr StringView& operator=(const StringView& other)
    {
        m_Characters = other.m_Characters;
        m_ByteCount = other.m_ByteCount;
        return *this;
    }

    FORCEINLINE constexpr StringView& operator=(StringView&& other) noexcept
    {
        m_Characters = other.m_Characters;
        m_ByteCount = other.m_ByteCount;
        other.m_Characters = nullptr;
        other.m_ByteCount = 0;
        return *this;
    }

public:
    NODISCARD FORCEINLINE usize ByteCount() const { return m_ByteCount; }
    NODISCARD FORCEINLINE const char* Characters() const { return m_Characters; }

    NODISCARD FORCEINLINE bool IsEmpty() const { return m_ByteCount == 0; }
    NODISCARD FORCEINLINE bool HasCharacters() const { return m_ByteCount > 0; }

public:
    NODISCARD FORCEINLINE bool operator==(const StringView& other) const
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

    NODISCARD FORCEINLINE bool operator!=(const StringView& other) const
    {
        const bool areEqual = ((*this) == other);
        return !areEqual;
    }

private:
    usize m_ByteCount;
    const char* m_Characters;
};

}
