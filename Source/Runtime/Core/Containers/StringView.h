// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

#define VIEW(string_literal) ::SE::StringView::FromUTF8(string_literal, sizeof(string_literal) - 1)

namespace SE
{

class StringView
{
public:
    NODISCARD FORCEINLINE static StringView FromUTF8(const char* characters, usize byteCount)
    {
        StringView view;
        view.m_Characters = characters;
        view.m_ByteCount = byteCount;
        return view;
    }

    NODISCARD SHOOTER_API static StringView FromUTF8(const char* nullTerminatedCharacters);

public:
    FORCEINLINE StringView()
        : m_Characters(nullptr)
        , m_ByteCount(0)
    {}

    FORCEINLINE StringView(const StringView& other)
        : m_Characters(other.m_Characters)
        , m_ByteCount(other.m_ByteCount)
    {}

    FORCEINLINE StringView(StringView&& other) noexcept
        : m_Characters(other.m_Characters)
        , m_ByteCount(other.m_ByteCount)
    {
        other.m_Characters = nullptr;
        other.m_ByteCount = 0;
    }

    FORCEINLINE StringView& operator=(const StringView& other)
    {
        m_Characters = other.m_Characters;
        m_ByteCount = other.m_ByteCount;
        return *this;
    }

    FORCEINLINE StringView& operator=(StringView&& other) noexcept
    {
        m_Characters = other.m_Characters;
        m_ByteCount = other.m_ByteCount;
        other.m_Characters = nullptr;
        other.m_ByteCount = 0;
        return *this;
    }

public:
    NODISCARD FORCEINLINE const char* Characters() const { return m_Characters; }
    NODISCARD FORCEINLINE usize ByteCount() const { return m_ByteCount; }

    NODISCARD FORCEINLINE bool IsEmpty() const { return m_ByteCount == 0; }
    NODISCARD FORCEINLINE bool HasCharacters() const { return m_ByteCount > 0; }

private:
    const char* m_Characters;
    usize m_ByteCount;
};

}
