/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Runtime/Core/Math/Vector.h>
#include <Runtime/Renderer/RHI/Texture.h>
#include <Runtime/Scene/Reflection/ReflectionComponent.h>

namespace SE
{

SE_REFLECTION_COMPONENT_METADATA(SpriteRendererComponent, 0xD406A616511BEA69);
struct SpriteRendererComponent
{
    RefPtr<Texture2D> Texture;
    Vector4           TintColor    = { 1.0F, 1.0F, 1.0F, 1.0F };
    float             TilingFactor = 1.0F;
};

} // namespace SE
