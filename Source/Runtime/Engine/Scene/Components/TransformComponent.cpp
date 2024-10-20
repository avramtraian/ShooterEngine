/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/ComponentRegistry.h>
#include <Engine/Scene/ComponentRegistryFields.h>
#include <Engine/Scene/Components/TransformComponent.h>

namespace SE
{

UUID TransformComponent::get_static_component_type_uuid()
{
    // NOTE: All engine components have their type UUID's manually generated.
    constexpr UUID transform_component_type_uuid = UUID(0xFC0BD5C5335C9E5B);
    return transform_component_type_uuid;
}

void TransformComponent::on_register(ComponentRegisterBuilder& register_builder) const
{
    PFN_ComponentConstruct construct_function = [](void* address, const EntityComponentInitializer& initializer)
    {
        new (address) TransformComponent(initializer);
    };

    register_builder.set_type_uuid(get_static_component_type_uuid());
    register_builder.set_parent_type_uuid(Super::get_static_component_type_uuid());
    register_builder.set_name("TransformComponent"sv);
    register_builder.set_construct_function(construct_function);

    SE_TRY_ADD_COMPONENT_FIELD(m_translation);
    SE_TRY_ADD_COMPONENT_FIELD(m_rotation);
    SE_TRY_ADD_COMPONENT_FIELD(m_scale);
}

TransformComponent::TransformComponent(const EntityComponentInitializer& initializer, Vector3 in_translation, Vector3 in_rotation, Vector3 in_scale)
    : EntityComponent(initializer)
    , m_translation(in_translation)
    , m_scale(in_scale)
    , m_rotation(in_rotation)
{}

} // namespace SE
