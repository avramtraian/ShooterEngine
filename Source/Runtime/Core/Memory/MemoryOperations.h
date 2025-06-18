// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

SHOOTER_API void copy_memory(void* destination, const void* source, usize byte_count);

SHOOTER_API void set_memory(void* destination, uint8 byte_value, usize byte_count);

SHOOTER_API void zero_memory(void* destination, usize byte_count);

}
