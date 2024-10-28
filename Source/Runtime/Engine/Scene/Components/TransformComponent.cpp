/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/MatrixTransformations.h>
#include <Engine/Scene/Components/TransformComponent.h>
#include <Engine/Scene/Reflection/ComponentReflector.h>

namespace SE
{

UUID TransformComponent::get_static_component_type_uuid()
{
    // NOTE: All engine components have their type UUID's manually generated.
    constexpr UUID transform_component_type_uuid = UUID(0xFC0BD5C5335C9E5B);
    return transform_component_type_uuid;
}

void TransformComponent::on_register(ComponentReflector& reflector)
{
    PFN_InstantiateComponent instantiate_function = [](void* address, const EntityComponentInitializer& initializer)
    {
        new (address) TransformComponent(initializer);
    };

    reflector.parent_type_uuid = Super::get_static_component_type_uuid();
    reflector.structure_byte_count = sizeof(TransformComponent);
    reflector.name = "TransformComponent"sv;
    reflector.instantiate_function = instantiate_function;

    {
        ComponentField& field = reflector.fields.emplace();
        field.type_stack.add(ComponentFieldType::Vector3);
        field.byte_offset = SE_OFFSET_OF(TransformComponent, m_translation);
        field.name = "m_translation"sv;
    }
    {
        ComponentField& field = reflector.fields.emplace();
        field.type_stack.add(ComponentFieldType::Vector3);
        field.byte_offset = SE_OFFSET_OF(TransformComponent, m_rotation);
        field.name = "m_rotation"sv;
        field.metadata.add_flag(ComponentFieldFlag::DisplayInDegrees);
    }
    {
        ComponentField& field = reflector.fields.emplace();
        field.type_stack.add(ComponentFieldType::Vector3);
        field.byte_offset = SE_OFFSET_OF(TransformComponent, m_scale);
        field.name = "m_scale"sv;
    }
}

TransformComponent::TransformComponent(const EntityComponentInitializer& initializer, Vector3 in_translation, Vector3 in_rotation, Vector3 in_scale)
    : EntityComponent(initializer)
    , m_translation(in_translation)
    , m_scale(in_scale)
    , m_rotation(in_rotation)
{}

Matrix4 TransformComponent::get_transform_matrix() const
{
    const Matrix4 translation_matrix = Matrix4::translate(m_translation);
    const Matrix4 rotation_matrix = Matrix4::rotate(m_rotation);
    const Matrix4 scale_matrix = Matrix4::scale(m_scale);
    return translation_matrix * rotation_matrix * scale_matrix;
}

} // namespace SE
