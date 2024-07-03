/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/Span.h"
#include "Core/EngineAPI.h"

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

    ALWAYS_INLINE Buffer(const void* in_bytes, usize in_byte_count)
        : m_bytes((ReadWriteBytes)(in_bytes))
        , m_byte_count(in_byte_count)
    {}

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

    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_byte_count; }
    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_byte_count == 0); }

    NODISCARD ALWAYS_INLINE ReadWriteByteSpan span() const { return ReadWriteByteSpan(bytes(), byte_count()); }
    NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_span() const { return ReadonlyByteSpan(readonly_bytes(), byte_count()); }

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

class ScopedBuffer
{
    SE_MAKE_NONCOPYABLE(ScopedBuffer);
    SE_MAKE_NONMOVABLE(ScopedBuffer);

public:
    ScopedBuffer() = default;

    ALWAYS_INLINE ~ScopedBuffer()
    {
        release();
    }

public:
    NODISCARD ALWAYS_INLINE ReadWriteBytes bytes() const { return m_buffer.bytes(); }
    NODISCARD ALWAYS_INLINE ReadonlyBytes readonly_bytes() const { return m_buffer.readonly_bytes(); }
    NODISCARD ALWAYS_INLINE ReadWriteBytes operator*() const { return *m_buffer; }

    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_buffer.byte_count(); }
    NODISCARD ALWAYS_INLINE bool is_empty() const { return m_buffer.is_empty(); }

    NODISCARD ALWAYS_INLINE ReadWriteByteSpan span() const { return m_buffer.span(); }
    NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_span() const { return m_buffer.readonly_span(); }

    template<typename T>
    NODISCARD ALWAYS_INLINE T* as() const { return m_buffer.as<T>(); }

public:
    // The buffer must be empty before calling this function. Otherwise, an assert will be triggered.
    ALWAYS_INLINE void allocate(usize in_byte_count) { m_buffer.allocate(in_byte_count); }
    ALWAYS_INLINE void release() { m_buffer.release(); }
    ALWAYS_INLINE void re_allocate(usize in_byte_count) { m_buffer.re_allocate(in_byte_count); }

private:
    Buffer m_buffer;
};

} // namespace SE