/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/Components/TransformComponent.h>

namespace SE
{

UUID TransformComponent::get_static_component_type_uuid()
{
    // NOTE: All engine components have their type UUID's manually generated.
    constexpr UUID transform_component_type_uuid = UUID(0xFC0BD5C5335C9E5B);
    return transform_component_type_uuid;
}

TransformComponent::TransformComponent(const EntityComponentInitializer& initializer, Vector3 in_translation, Vector3 in_rotation, Vector3 in_scale)
    : EntityComponent(initializer)
    , m_translation(in_translation)
    , m_scale(in_scale)
    , m_rotation(in_rotation)
{}

} // namespace SE
