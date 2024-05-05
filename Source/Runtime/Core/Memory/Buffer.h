/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/Span.h"

namespace SE
{

class Buffer
{
public:
    static Buffer copy(const Buffer& source);

public:
    ALWAYS_INLINE Buffer()
        : m_bytes(nullptr)
        , m_byte_count(0)
    {}

    ALWAYS_INLINE Buffer(const Buffer& other)
        : m_bytes(other.m_bytes)
        , m_byte_count(other.m_byte_count)
    {}

    ALWAYS_INLINE Buffer(Buffer&& other) noexcept
        : m_bytes(other.m_bytes)
        , m_byte_count(other.m_byte_count)
    {
        other.m_bytes = nullptr;
        other.m_byte_count = 0;
    }

    ALWAYS_INLINE Buffer& operator=(const Buffer& other)
    {
        m_bytes = other.m_bytes;
        m_byte_count = other.m_byte_count;
        return *this;
    }

    ALWAYS_INLINE Buffer& operator=(Buffer&& other) noexcept
    {
        m_bytes = other.m_bytes;
        m_byte_count = other.m_byte_count;
        other.m_bytes = nullptr;
        other.m_byte_count = 0;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE ReadWriteBytes bytes() const { return m_bytes; }
    NODISCARD ALWAYS_INLINE ReadonlyBytes readonly_bytes() const { return m_bytes; }
    NODISCARD ALWAYS_INLINE ReadWriteBytes operator*() const { return bytes(); }

    NODISCARD ALWAYS_INLINE usize count() const { return m_byte_count; }
    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_byte_count; }
    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_byte_count == 0); }

    NODISCARD ALWAYS_INLINE ReadWriteByteSpan span() const { return ReadWriteByteSpan(bytes(), count()); }
    NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_span() const { return ReadonlyByteSpan(readonly_bytes(), count()); }

    template<typename T>
    NODISCARD ALWAYS_INLINE T* as() const { return (T*)(m_bytes); }

public:
    // The buffer must be empty before calling this function. Otherwise, an assert will be triggered.
    SHOOTER_API void allocate(usize in_byte_count);
    
    SHOOTER_API void release();
    
    ALWAYS_INLINE void re_allocate(usize in_byte_count)
    {
        release();
        allocate(in_byte_count);
    }

private:
    ReadWriteBytes m_bytes;
    usize m_byte_count;
};

} // namespace SE