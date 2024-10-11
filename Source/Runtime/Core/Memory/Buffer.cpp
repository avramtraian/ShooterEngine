/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Memory/Buffer.h>
#include <Core/Memory/MemoryOperations.h>

namespace SE
{

Buffer::Buffer(Buffer&& other) noexcept
    : m_bytes(other.m_bytes)
    , m_byte_count(other.m_byte_count)
{
    other.m_bytes = nullptr;
    other.m_byte_count = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept
{
    if (this == &other)
        return *this;

    release();

    m_bytes = other.m_bytes;
    m_byte_count = other.m_byte_count;

    other.m_bytes = nullptr;
    other.m_byte_count = 0;

    return *this;
}

Buffer::~Buffer()
{
    release();
}

Buffer Buffer::copy(const Buffer& source_buffer)
{
    Buffer destination_buffer;
    destination_buffer.allocate(source_buffer.m_byte_count);
    copy_memory_from_span(destination_buffer.m_bytes, source_buffer.readonly_span());
    return destination_buffer;
}

void Buffer::allocate(usize capacity_in_bytes)
{
    release();
    m_byte_count = capacity_in_bytes;
    m_bytes = static_cast<ReadWriteBytes>(operator new(m_byte_count));
}

void Buffer::expand(usize new_capacity_in_bytes)
{
    SE_ASSERT(new_capacity_in_bytes >= m_byte_count);

    ReadWriteBytes new_block = static_cast<ReadWriteBytes>(operator new(new_capacity_in_bytes));
    copy_memory(new_block, m_bytes, m_byte_count);

    release();
    m_bytes = new_block;
    m_byte_count = new_capacity_in_bytes;
}

void Buffer::release()
{
    if (m_byte_count > 0)
    {
        delete m_bytes;
        m_bytes = nullptr;
        m_byte_count = 0;
    }
}

} // namespace SE
