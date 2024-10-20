/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Engine/Scene/ComponentRegistry.h>
#include <Engine/Scene/Components/SpriteRendererComponent.h>
#include <Engine/Scene/Components/TransformComponent.h>
#include <Engine/Scene/EntityComponent.h>

namespace SE
{

bool ComponentRegistry::initialize()
{
    construct_engine_components_registers();
    return true;
}

void ComponentRegistry::shutdown()
{
    m_component_registry.clear_and_shrink();
}

void ComponentRegistry::construct_register(const ComponentRegisterBuilder& builder)
{
    ComponentRegisterData& register_data = m_component_registry.get_or_add(builder.get_type_uuid());
    register_data.type_uuid = builder.get_type_uuid();
    register_data.structure_byte_count = builder.get_structure_byte_count();
    register_data.parent_uuid = builder.get_parent_type_uuid();
    register_data.name = builder.get_name();
    register_data.construct_function = builder.get_construct_function();
    register_data.fields = builder.get_fields();

    if (builder.get_parent_type_uuid() != UUID::invalid())
    {
        ComponentRegisterData& parent_register_data = m_component_registry.get_or_add(builder.get_parent_type_uuid());
        parent_register_data.children_uuids.add(builder.get_type_uuid());
    }
}

void ComponentRegistry::construct_engine_components_registers()
{
    EntityComponentInitializer default_component_initializer = {};
    default_component_initializer.parent_entity = nullptr;
    default_component_initializer.scene_context = nullptr;

    // TransformComponent
    {
        TransformComponent transform_component_default_object = TransformComponent(default_component_initializer);
        ComponentRegisterBuilder register_builder;
        transform_component_default_object.on_register(register_builder);
        construct_register(register_builder);
    }

    // SpriteRendererComponent
    {
        SpriteRendererComponent sprite_renderer_component_default_object = SpriteRendererComponent(default_component_initializer);
        ComponentRegisterBuilder register_builder;
        sprite_renderer_component_default_object.on_register(register_builder);
        construct_register(register_builder);
    }
}

} // namespace SE
