/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/Memory/Buffer.h"
#include "Core/Memory/MemoryOperations.h"

namespace SE
{

Buffer Buffer::copy(const Buffer& source)
{
    Buffer destination;
    destination.allocate(source.m_byte_count);
    copy_memory(*destination, *source, destination.m_byte_count);
    return destination;
}

void Buffer::allocate(usize in_byte_count)
{
    SE_ASSERT(is_empty());
    m_byte_count = in_byte_count;
    m_bytes = (ReadWriteBytes)(::operator new(m_byte_count));
}

void Buffer::release()
{
    if (m_byte_count == 0)
        return;

    ::operator delete(m_bytes);
    m_bytes = nullptr;
    m_byte_count = 0;
}

} // namespace SE