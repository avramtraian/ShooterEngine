// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

SHOOTER_API void MemoryCopy(void* destination, const void* source, usize byteCount);

SHOOTER_API void MemorySet(void* destination, uint8 byteValue, usize byteCount);

SHOOTER_API void MemoryZero(void* destination, usize byteCount);

}
