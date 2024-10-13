/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/Span.h>
#include <Core/CoreTypes.h>

namespace SE
{

class Buffer
{
    SE_MAKE_NONCOPYABLE(Buffer);

public:
    SHOOTER_API NODISCARD static Buffer create(usize initial_byte_count);

    SHOOTER_API NODISCARD static Buffer copy(const void* initial_data, usize initial_byte_count);

    NODISCARD ALWAYS_INLINE static Buffer copy(const Buffer& source) { return Buffer::copy(source.data(), source.byte_count()); }

public:
    SHOOTER_API Buffer();
    SHOOTER_API ~Buffer();

    SHOOTER_API Buffer(Buffer&& other) noexcept;
    SHOOTER_API Buffer& operator=(Buffer&& other) noexcept;

public:
    NODISCARD ALWAYS_INLINE void* data() { return m_data; }
    NODISCARD ALWAYS_INLINE const void* data() const { return m_data; }

    NODISCARD ALWAYS_INLINE ReadWriteBytes bytes() { return static_cast<ReadWriteBytes>(m_data); }
    NODISCARD ALWAYS_INLINE ReadonlyBytes bytes() const { return static_cast<ReadonlyBytes>(m_data); }

    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_byte_count; }
    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_byte_count == 0); }

    NODISCARD ALWAYS_INLINE ReadWriteByteSpan byte_span() { return ReadWriteByteSpan(static_cast<u8*>(m_data), m_byte_count); }
    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span() const { return ReadonlyByteSpan(static_cast<const u8*>(m_data), m_byte_count); }
    NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_byte_span() const { return ReadonlyByteSpan(static_cast<const u8*>(m_data), m_byte_count); }

public:
    template<typename T>
    NODISCARD ALWAYS_INLINE T* as()
    {
        T* casted_data = static_cast<T*>(m_data);
        return casted_data;
    }

    template<typename T>
    NODISCARD ALWAYS_INLINE const T* as() const
    {
        T* casted_data = static_cast<const T*>(m_data);
        return casted_data;
    }

public:
    SHOOTER_API void allocate_new(usize new_byte_count);

    SHOOTER_API void expand(usize new_byte_count);

    SHOOTER_API void release();

private:
    void* m_data;
    usize m_byte_count;
};

} // namespace SE
