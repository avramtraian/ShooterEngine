/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/Span.h>

#include <new>

namespace SE
{

SHOOTER_API void copy_memory(void* destination, const void* source, usize byte_count);

SHOOTER_API void set_memory(void* destination, ReadonlyByte byte_value, usize byte_count);

SHOOTER_API void zero_memory(void* destination, usize byte_count);

template<typename T>
ALWAYS_INLINE static void copy_memory_from_span(void* destination, Span<T> span)
{
    copy_memory(destination, span.elements(), span.count() * span.element_size());
}

} // namespace SE
