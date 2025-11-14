/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Runtime/Core/Math/Vector.h>
#include <Runtime/Scene/Reflection/ReflectionComponent.h>

namespace SE
{

SE_REFLECTION_COMPONENT_METADATA(TransformComponent, 0x0CB707595FEE56DD);
struct TransformComponent
{
    Vector3 Translation = { 0.0F, 0.0F, 0.0F };
    Vector3 Rotation    = { 0.0F, 0.0F, 0.0F };
    Vector3 Scale       = { 1.0F, 1.0F, 1.0F };
};

} // namespace SE
