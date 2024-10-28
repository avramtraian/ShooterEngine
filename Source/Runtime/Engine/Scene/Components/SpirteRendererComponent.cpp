/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/Components/SpriteRendererComponent.h>
#include <Engine/Scene/Reflection/ComponentReflector.h>

namespace SE
{

UUID SpriteRendererComponent::get_static_component_type_uuid()
{
    // NOTE: All engine components have their type UUID's manually generated.
    constexpr UUID sprite_renderer_component_type_uuid = UUID(0x529DE4F13FEECFD0);
    return sprite_renderer_component_type_uuid;
}

void SpriteRendererComponent::on_register(ComponentReflector& reflector)
{
    PFN_InstantiateComponent instantiate_function = [](void* address, const EntityComponentInitializer& initializer)
    {
        new (address) SpriteRendererComponent(initializer);
    };

    reflector.parent_type_uuid = Super::get_static_component_type_uuid();
    reflector.structure_byte_count = sizeof(SpriteRendererComponent);
    reflector.name = "SpriteRendererComponent"sv;
    reflector.instantiate_function = instantiate_function;

    {
        ComponentField& field = reflector.fields.emplace();
        field.type_stack.add(ComponentFieldType::Color4);
        field.byte_offset = SE_OFFSET_OF(SpriteRendererComponent, m_sprite_color);
        field.name = "m_translation"sv;
    }
}

SpriteRendererComponent::SpriteRendererComponent(const EntityComponentInitializer& initializer, Color4 in_sprite_color)
    : EntityComponent(initializer)
    , m_sprite_color(in_sprite_color)
{}

} // namespace SE
