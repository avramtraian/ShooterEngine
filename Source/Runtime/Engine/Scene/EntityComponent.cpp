/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/Entity.h>
#include <Engine/Scene/EntityComponent.h>

namespace SE
{

EntityComponent::EntityComponent(const EntityComponentInitializer& initializer)
    : m_parent_entity(*initializer.parent_entity)
    , m_is_updatable(true)
{}

Scene& EntityComponent::scene_context()
{
    return m_parent_entity.scene_context();
}

const Scene& EntityComponent::scene_context() const
{
    return m_parent_entity.scene_context();
}

void EntityComponent::set_is_updatable(bool is_updatable)
{
    m_is_updatable = is_updatable;
}

} // namespace SE
