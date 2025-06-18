// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>

namespace SE
{

void MemoryCopy(void* destination, const void* source, usize byteCount)
{
    const WriteonlyBytes dst = static_cast<WriteonlyBytes>(destination);
    const ReadonlyBytes src = static_cast<ReadonlyBytes>(source);

    for (usize byte_offset = 0; byte_offset < byteCount; ++byte_offset)
        dst[byte_offset] = src[byte_offset];
}

void MemorySet(void* destination, uint8 byte_value, usize byteCount)
{
    const WriteonlyBytes dst = static_cast<WriteonlyBytes>(destination);

    for (usize byte_offset = 0; byte_offset < byteCount; ++byte_offset)
        dst[byte_offset] = byte_value;
}

void MemoryZero(void* destination, usize byteCount)
{
    const WriteonlyBytes dst = static_cast<WriteonlyBytes>(destination);

    for (usize byte_offset = 0; byte_offset < byteCount; ++byte_offset)
        dst[byte_offset] = 0;
}

}
