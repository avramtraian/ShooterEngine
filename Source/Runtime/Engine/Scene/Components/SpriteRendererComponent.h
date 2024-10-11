/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Math/Color.h>
#include <Engine/Scene/EntityComponent.h>

namespace SE
{

class SpriteRendererComponent : public EntityComponent
{
    SE_ENGINE_ENTITY_COMPONENT(SpriteRendererComponent, EntityComponent);

public:
    SHOOTER_API SpriteRendererComponent(const EntityComponentInitializer&, Color4 in_sprite_color);

public:
    NODISCARD ALWAYS_INLINE Color4 sprite_color() const { return m_sprite_color; }
    ALWAYS_INLINE void set_sprite_color(Color4 new_sprite_color) { m_sprite_color = new_sprite_color; }

private:
    Color4 m_sprite_color { 1, 1, 1, 1 };
};

} // namespace SE
