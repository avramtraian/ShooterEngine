// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

#define VIEW(string_literal) ::SE::StringView::from_utf8(string_literal, sizeof(string_literal) - 1)

namespace SE
{

class StringView
{
public:
    NODISCARD FORCEINLINE static StringView from_utf8(const char* characters, usize byte_count)
    {
        StringView view;
        view.m_characters = characters;
        view.m_byte_count = byte_count;
        return view;
    }

    NODISCARD SHOOTER_API static StringView from_utf8(const char* null_terminated_characters);

public:
    FORCEINLINE StringView()
        : m_characters(nullptr)
        , m_byte_count(0)
    {}

    FORCEINLINE StringView(const StringView& other)
        : m_characters(other.m_characters)
        , m_byte_count(other.m_byte_count)
    {}

    FORCEINLINE StringView(StringView&& other) noexcept
        : m_characters(other.m_characters)
        , m_byte_count(other.m_byte_count)
    {
        other.m_characters = nullptr;
        other.m_byte_count = 0;
    }

    FORCEINLINE StringView& operator=(const StringView& other)
    {
        m_characters = other.m_characters;
        m_byte_count = other.m_byte_count;
        return *this;
    }

    FORCEINLINE StringView& operator=(StringView&& other) noexcept
    {
        m_characters = other.m_characters;
        m_byte_count = other.m_byte_count;
        other.m_characters = nullptr;
        other.m_byte_count = 0;
        return *this;
    }

public:
    NODISCARD FORCEINLINE const char* characters() const { return m_characters; }
    NODISCARD FORCEINLINE usize byte_count() const { return m_byte_count; }

    NODISCARD FORCEINLINE bool is_empty() const { return m_byte_count == 0; }
    NODISCARD FORCEINLINE bool has_characters() const { return m_byte_count > 0; }

private:
    const char* m_characters;
    usize m_byte_count;
};

}
