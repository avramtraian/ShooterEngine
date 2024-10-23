/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/MatrixTransformations.h>
#include <Engine/Scene/ComponentRegistry.h>
#include <Engine/Scene/ComponentRegistryFields.h>
#include <Engine/Scene/Components/CameraComponent.h>
#include <Engine/Scene/Entity.h>
#include <Engine/Scene/Scene.h>

namespace SE
{

UUID CameraComponent::get_static_component_type_uuid()
{
    // NOTE: All engine components have their type UUID's manually generated.
    constexpr UUID transform_component_type_uuid = UUID(0xB7AF50D750888405);
    return transform_component_type_uuid;
}

void CameraComponent::on_register(ComponentRegisterBuilder& register_builder) const
{
    PFN_ComponentConstruct construct_function = [](void* address, const EntityComponentInitializer& initializer)
    {
        new (address) CameraComponent(initializer);
    };

    register_builder.set_type_uuid(get_static_component_type_uuid());
    register_builder.set_structure_byte_count(sizeof(CameraComponent));
    register_builder.set_parent_type_uuid(Super::get_static_component_type_uuid());
    register_builder.set_name("CameraComponent"sv);
    register_builder.set_construct_function(construct_function);

    SE_TRY_ADD_COMPONENT_FIELD(m_is_primary);
    register_builder.push_field_flag(ComponentFieldFlag::Angle);
    SE_TRY_ADD_COMPONENT_FIELD(m_vertical_field_of_view);
    SE_TRY_ADD_COMPONENT_FIELD(m_clip_plane_near);
    SE_TRY_ADD_COMPONENT_FIELD(m_clip_plane_far);
}

Matrix4 CameraComponent::get_projection_matrix(float aspect_ratio) const
{
    return Matrix4::perspective(m_vertical_field_of_view, aspect_ratio, m_clip_plane_near, m_clip_plane_far);
}

void CameraComponent::on_update(float delta_time)
{
    Scene& scene = scene_context();
    if (m_is_primary && scene.get_primary_camera_entity_uuid() == UUID::invalid())
    {
        // Set the parent entity as the primary camera entity in the scene.
        scene_context().set_primary_camera_entity(parent_entity()->uuid());
    }
}

} // namespace SE
