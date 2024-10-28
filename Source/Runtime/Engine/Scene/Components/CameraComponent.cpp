/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/MatrixTransformations.h>
#include <Engine/Scene/Components/CameraComponent.h>
#include <Engine/Scene/Entity.h>
#include <Engine/Scene/Reflection/ComponentReflector.h>
#include <Engine/Scene/Scene.h>

namespace SE
{

UUID CameraComponent::get_static_component_type_uuid()
{
    // NOTE: All engine components have their type UUID's manually generated.
    constexpr UUID transform_component_type_uuid = UUID(0xB7AF50D750888405);
    return transform_component_type_uuid;
}

void CameraComponent::on_register(ComponentReflector& reflector)
{
    PFN_InstantiateComponent instantiate_function = [](void* address, const EntityComponentInitializer& initializer)
    {
        new (address) CameraComponent(initializer);
    };

    reflector.parent_type_uuid = Super::get_static_component_type_uuid();
    reflector.structure_byte_count = sizeof(CameraComponent);
    reflector.name = "CameraComponent"sv;
    reflector.instantiate_function = instantiate_function;

    {
        ComponentField& field = reflector.fields.emplace();
        field.type_stack.add(ComponentFieldType::Boolean);
        field.byte_offset = SE_OFFSET_OF(CameraComponent, m_is_primary);
        field.name = "m_is_primary"sv;
    }
    {
        ComponentField& field = reflector.fields.emplace();
        field.type_stack.add(ComponentFieldType::Float32);
        field.byte_offset = SE_OFFSET_OF(CameraComponent, m_vertical_field_of_view);
        field.name = "m_vertical_field_of_view"sv;
        field.metadata.add_flag(ComponentFieldFlag::DisplayInDegrees);
    }
    {
        ComponentField& field = reflector.fields.emplace();
        field.type_stack.add(ComponentFieldType::Float32);
        field.byte_offset = SE_OFFSET_OF(CameraComponent, m_clip_plane_near);
        field.name = "m_clip_plane_near"sv;
    }
    {
        ComponentField& field = reflector.fields.emplace();
        field.type_stack.add(ComponentFieldType::Float32);
        field.byte_offset = SE_OFFSET_OF(CameraComponent, m_clip_plane_far);
        field.name = "m_clip_plane_far"sv;
    }
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
