// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>

namespace SE
{

void copy_memory(void* destination, const void* source, usize byte_count)
{
    const WriteonlyBytes dst = static_cast<WriteonlyBytes>(destination);
    const ReadonlyBytes src = static_cast<ReadonlyBytes>(source);

    for (usize byte_offset = 0; byte_offset < byte_count; ++byte_offset)
    {
        dst[byte_offset] = src[byte_offset];
    }
}

void set_memory(void* destination, u8 byte_value, usize byte_count)
{
    const WriteonlyBytes dst = static_cast<WriteonlyBytes>(destination);

    for (usize byte_offset = 0; byte_offset < byte_count; ++byte_offset)
    {
        dst[byte_offset] = byte_value;
    }
}

void zero_memory(void* destination, usize byte_count)
{
    const WriteonlyBytes dst = static_cast<WriteonlyBytes>(destination);

    for (usize byte_offset = 0; byte_offset < byte_count; ++byte_offset)
    {
        dst[byte_offset] = 0;
    }
}

}
