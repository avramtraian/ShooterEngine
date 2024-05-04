/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Assertions.h"
#include "Core/CoreTypes.h"

namespace SE {

template<typename T>
class Span {
public:
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = T*;
    using ReverseConstIterator = const T*;

public:
    ALWAYS_INLINE constexpr Span()
        : m_elements(nullptr)
        , m_count(0)
    {}

    ALWAYS_INLINE constexpr Span(T* in_elements, usize in_count)
        : m_elements(in_elements)
        , m_count(in_count)
    {}

    ALWAYS_INLINE constexpr Span(Span&& other) noexcept
        : m_elements(other.m_elements)
        , m_count(other.m_count)
    {
        other.m_elements = nullptr;
        other.m_count = 0;
    }

    ALWAYS_INLINE Span& operator=(Span&& other) noexcept
    {
        m_elements = other.m_elements;
        m_count = other.m_count;
        other.m_elements = nullptr;
        other.m_count = 0;
        return *this;
    }

    Span(const Span&) = default;
    Span& operator=(const Span&) = default;

public:
    NODISCARD ALWAYS_INLINE T* elements() { return m_elements; }
    NODISCARD ALWAYS_INLINE const T* elements() const { return m_elements; }

    NODISCARD ALWAYS_INLINE T* operator*() { return elements(); }
    NODISCARD ALWAYS_INLINE const T* operator*() const { return elements(); }

    NODISCARD ALWAYS_INLINE usize count() const { return m_count; }
    NODISCARD ALWAYS_INLINE static constexpr usize element_size() { return sizeof(T); }

    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_count == 0); }
    NODISCARD ALWAYS_INLINE bool has_elements() const { return (m_count > 0); }

public:
    NODISCARD ALWAYS_INLINE T& at(usize index)
    {
        SE_ASSERT(index < m_count);
        return m_elements[index];
    }

    NODISCARD ALWAYS_INLINE const T& at(usize index) const
    {
        SE_ASSERT(index < m_count);
        return m_elements[index];
    }

    NODISCARD ALWAYS_INLINE T& operator[](usize index) { return at(index); }
    NODISCARD ALWAYS_INLINE const T& operator[](usize index) const { return at(index); }


public:
    NODISCARD ALWAYS_INLINE Iterator begin() { return m_elements; }
    NODISCARD ALWAYS_INLINE Iterator end() { return m_elements + m_count; }

    NODISCARD ALWAYS_INLINE ConstIterator begin() const { return m_elements; }
    NODISCARD ALWAYS_INLINE ConstIterator end() const { return m_elements + m_count; }

    NODISCARD ALWAYS_INLINE ReverseIterator rbegin() { return m_elements + m_count - 1; }
    NODISCARD ALWAYS_INLINE ReverseIterator rend() { return m_elements - 1; }

    NODISCARD ALWAYS_INLINE ReverseConstIterator rbegin() const { return m_elements + m_count - 1; }
    NODISCARD ALWAYS_INLINE ReverseConstIterator rend() const { return m_elements - 1; }

private:
    T* m_elements;
    usize m_count;
};

using ReadonlyByteSpan = Span<ReadonlyByte>;
using WriteonlyByteSpan = Span<WriteonlyByte>;
using ReadWriteByteSpan = Span<ReadWriteByte>;

} // namespace SE