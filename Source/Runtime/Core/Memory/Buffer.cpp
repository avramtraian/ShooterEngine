/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Assertions.h>
#include <Core/Memory/Buffer.h>
#include <Core/Memory/MemoryOperations.h>

namespace SE
{

Buffer Buffer::create(usize initial_byte_count)
{
    Buffer result;
    result.allocate_new(initial_byte_count);
    return result;
}

Buffer Buffer::copy(const void* initial_data, usize initial_byte_count)
{
    Buffer result;
    result.allocate_new(initial_byte_count);
    copy_memory(result.data(), initial_data, result.byte_count());
    return result;
}

Buffer::Buffer()
    : m_data(nullptr)
    , m_byte_count(0)
{}

Buffer::~Buffer()
{
    release();
}

Buffer::Buffer(Buffer&& other) noexcept
    : m_data(other.m_data)
    , m_byte_count(other.m_byte_count)
{
    other.m_data = nullptr;
    other.m_byte_count = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept
{
    // Handle self-assignment case.
    if (this == &other)
        return *this;

    release();

    m_data = other.m_data;
    m_byte_count = other.m_byte_count;

    other.m_data = nullptr;
    other.m_byte_count = 0;

    return *this;
}

void Buffer::allocate_new(usize new_byte_count)
{
    release();

    m_byte_count = new_byte_count;
    m_data = ::operator new(m_byte_count);

    // NOTE: Depending on the platform it is not guaranteed that the memory provided by the
    // default heap allocator is zero-initialized.
    zero_memory(m_data, m_byte_count);
}

void Buffer::expand(usize new_byte_count)
{
    SE_ASSERT(new_byte_count >= m_byte_count);
    if (new_byte_count == m_byte_count)
        return;

    void* new_data = ::operator new(new_byte_count);
    copy_memory(new_data, m_data, m_byte_count);

    // NOTE: Depending on the platform it is not guaranteed that the memory provided by the
    // default heap allocator is zero-initialized.
    zero_memory(static_cast<u8*>(new_data) + m_byte_count, new_byte_count - m_byte_count);

    release();
    m_data = new_data;
    m_byte_count = new_byte_count;
}

void Buffer::release()
{
    if (is_empty())
        return;

    ::operator delete(m_data);
    m_data = nullptr;
    m_byte_count = 0;
}

} // namespace SE
