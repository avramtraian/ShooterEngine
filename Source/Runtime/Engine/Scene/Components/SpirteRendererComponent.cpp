/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/Components/SpriteRendererComponent.h>

namespace SE
{

UUID SpriteRendererComponent::get_static_component_type_uuid()
{
    // NOTE: All engine components have their type UUID's manually generated.
    constexpr UUID sprite_renderer_component_type_uuid = UUID(0x529DE4F13FEECFD0);
    return sprite_renderer_component_type_uuid;
}

SpriteRendererComponent::SpriteRendererComponent(const EntityComponentInitializer& initializer, Color4 in_sprite_color)
    : EntityComponent(initializer)
    , m_sprite_color(in_sprite_color)
{}

} // namespace SE
