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
	SE_MAKE_NONCOPYABLE(Buffer);

public:
	Buffer() = default;

	SHOOTER_API Buffer(Buffer&& other) noexcept;
	SHOOTER_API Buffer& operator=(Buffer&& other) noexcept;

	SHOOTER_API ~Buffer();

	NODISCARD SHOOTER_API static Buffer copy(const Buffer& source_buffer);

public:
	NODISCARD ALWAYS_INLINE ReadWriteBytes bytes() { return m_bytes; }
	NODISCARD ALWAYS_INLINE ReadonlyBytes bytes() const { return m_bytes; }

	NODISCARD ALWAYS_INLINE usize byte_count() const { return m_byte_count; }
	NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_byte_count == 0); }

	NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_span() const { return ReadonlyByteSpan(bytes(), byte_count()); }
	NODISCARD ALWAYS_INLINE WriteonlyByteSpan writeonly_write_span() { return WriteonlyByteSpan(bytes(), byte_count()); }
	NODISCARD ALWAYS_INLINE ReadWriteByteSpan read_write_span() { return ReadWriteByteSpan(bytes(), byte_count()); }

	NODISCARD ALWAYS_INLINE ReadWriteByteSpan span() { return read_write_span(); }
	NODISCARD ALWAYS_INLINE ReadonlyByteSpan span() const { return readonly_span(); }

	template<typename T>
	NODISCARD ALWAYS_INLINE T* as() const
	{
		SE_ASSERT(sizeof(T) <= m_byte_count);
		return reinterpret_cast<T*>(m_bytes);
	}

public:
	SHOOTER_API void allocate(usize capacity_in_bytes);
	SHOOTER_API void expand(usize new_capacity_in_bytes);
	SHOOTER_API void release();

private:
	ReadWriteBytes m_bytes { nullptr };
	usize m_byte_count { 0 };
};

} // namespace SE
