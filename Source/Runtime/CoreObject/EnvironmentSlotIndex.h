// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

using EnvironmentSlotIndex = uint32;
using EnvironmentSlotGeneration = uint32;

constexpr EnvironmentSlotIndex INVALID_ENVIRONMENT_SLOT_INDEX = -1;
constexpr EnvironmentSlotGeneration INVALID_ENVIRONMENT_SLOT_GENERATION = -1;

}
