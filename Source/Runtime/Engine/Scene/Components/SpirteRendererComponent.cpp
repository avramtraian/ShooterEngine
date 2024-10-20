/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/ComponentRegistry.h>
#include <Engine/Scene/ComponentRegistryFields.h>
#include <Engine/Scene/Components/SpriteRendererComponent.h>

namespace SE
{

UUID SpriteRendererComponent::get_static_component_type_uuid()
{
    // NOTE: All engine components have their type UUID's manually generated.
    constexpr UUID sprite_renderer_component_type_uuid = UUID(0x529DE4F13FEECFD0);
    return sprite_renderer_component_type_uuid;
}

void SpriteRendererComponent::on_register(ComponentRegisterBuilder& register_builder) const
{
    PFN_ComponentConstruct construct_function = [](void* address, const EntityComponentInitializer& initializer)
    {
        new (address) SpriteRendererComponent(initializer);
    };

    register_builder.set_type_uuid(get_static_component_type_uuid());
    register_builder.set_structure_byte_count(sizeof(SpriteRendererComponent));
    register_builder.set_parent_type_uuid(Super::get_static_component_type_uuid());
    register_builder.set_name("SpriteRendererComponent"sv);
    register_builder.set_construct_function(construct_function);

    SE_TRY_ADD_COMPONENT_FIELD(m_sprite_color);
}

SpriteRendererComponent::SpriteRendererComponent(const EntityComponentInitializer& initializer, Color4 in_sprite_color)
    : EntityComponent(initializer)
    , m_sprite_color(in_sprite_color)
{}

} // namespace SE
