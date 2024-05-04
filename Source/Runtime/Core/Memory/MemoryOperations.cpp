/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/Memory/MemoryOperations.h"

namespace SE
{

SHOOTER_API void copy_memory(void* destination, const void* source, usize byte_count)
{
    WriteonlyBytes dst_bytes = reinterpret_cast<WriteonlyBytes>(destination);
    ReadonlyBytes src_bytes = reinterpret_cast<ReadonlyBytes>(destination);

    for (usize byte_offset = 0; byte_offset < byte_count; ++byte_offset)
        dst_bytes[byte_offset] = src_bytes[byte_offset];
}

SHOOTER_API void set_memory(void* destination, ReadonlyByte byte_value, usize byte_count)
{
    WriteonlyBytes dst_bytes = reinterpret_cast<WriteonlyBytes>(destination);
    for (usize byte_offset = 0; byte_offset < byte_count; ++byte_offset)
        dst_bytes[byte_offset] = byte_value;
}

SHOOTER_API void zero_memory(void* destination, usize byte_count)
{
    WriteonlyBytes dst_bytes = reinterpret_cast<WriteonlyBytes>(destination);
    for (usize byte_offset = 0; byte_offset < byte_count; ++byte_offset)
        dst_bytes[byte_offset] = 0;
}

} // namespace SE