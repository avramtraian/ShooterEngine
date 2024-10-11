/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/Entity.h>
#include <Engine/Scene/Scene.h>

namespace SE
{

Entity::Entity(Scene& in_scene_context, UUID entity_uuid)
    : m_scene_context(in_scene_context)
    , m_uuid(entity_uuid)
{}

Entity::~Entity()
{
    for (EntityComponent* component : m_components)
    {
        // NOTE: Any entity component is currently allocated by the default heap allocator.
        delete component;
    }

    m_components.clear_and_shrink();
}

void Entity::on_begin_play()
{
    for (EntityComponent* component : m_components)
    {
        component->on_begin_play();
    }
}

void Entity::on_end_play()
{
    for (EntityComponent* component : m_components)
    {
        component->on_end_play();
    }
}

void Entity::on_update(float delta_time)
{
    for (EntityComponent* component : m_components)
    {
        if (component->is_updatable())
            component->on_update(delta_time);
    }
}

bool Entity::has_component(UUID component_type_uuid) const
{
    for (const EntityComponent* component : m_components)
    {
        if (component->check_component_type_uuid(component_type_uuid))
            return true;
    }

    // The entity doesn't have the given component type (or a component derived from it).
    return false;
}

EntityComponent* Entity::get_component(UUID component_type_uuid)
{
    for (EntityComponent* component : m_components)
    {
        if (component->check_component_type_uuid(component_type_uuid))
            return component;
    }

    // The entity doesn't have the given component type (or a component derived from it).
    return nullptr;
}

const EntityComponent* Entity::get_component(UUID component_type_uuid) const
{
    for (const EntityComponent* component : m_components)
    {
        if (component->check_component_type_uuid(component_type_uuid))
            return component;
    }

    // The entity doesn't have the given component type (or a component derived from it).
    return nullptr;
}

void Entity::add_component(EntityComponent* component)
{
    m_components.add(component);

    if (m_scene_context.get_play_state() == Scene::PlayState::BeginPlaying || m_scene_context.get_play_state() == Scene::PlayState::Playing)
    {
        // If the scene is currently playing (even if just beginning to do so) invoke the component `on_begin_play` callback.
        component->on_begin_play();
    }
}

} // namespace SE
