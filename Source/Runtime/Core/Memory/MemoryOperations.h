// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

RUNTIME_API void MemoryCopy(void* destination, const void* source, usize byteCount);

RUNTIME_API void MemorySet(void* destination, uint8 byteValue, usize byteCount);

RUNTIME_API void MemoryZero(void* destination, usize byteCount);

}
