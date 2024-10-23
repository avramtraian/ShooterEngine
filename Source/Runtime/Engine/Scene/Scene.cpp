/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/Components/CameraComponent.h>
#include <Engine/Scene/Scene.h>

namespace SE
{

OwnPtr<Scene> Scene::create()
{
    Scene* scene = new Scene();
    return adopt_own(scene);
}

Scene::Scene()
    : m_play_state(PlayState::NotPlaying)
{}

Scene::~Scene()
{
    SE_ASSERT(m_play_state == PlayState::NotPlaying);
    m_entities.clear_and_shrink();
}

Entity* Scene::create_entity()
{
    // Entities can't be created during the `on_end_play` callback.
    SE_ASSERT(m_play_state != PlayState::EndPlaying);

    const UUID entity_uuid = UUID::create();
    return create_entity_with_uuid(entity_uuid);
}

Entity* Scene::create_entity_with_uuid(UUID entity_uuid)
{
    // Entities can't be created during the `on_end_play` callback.
    SE_ASSERT(m_play_state != PlayState::EndPlaying);

    Entity* entity = new Entity(*this, entity_uuid);
    m_entities.add(entity_uuid, adopt_own(entity));

    if (m_play_state == PlayState::BeginPlaying || m_play_state == PlayState::Playing)
    {
        // If the scene is currently playing (even if just beginning to do so) invoke the entity `on_begin_play` callback.
        entity->on_begin_play();
    }

    return entity;
}

Entity* Scene::get_entity_from_uuid(UUID entity_uuid)
{
    Optional<OwnPtr<Entity>&> entity = m_entities.get_if_exists(entity_uuid);
    return entity.has_value() ? entity.value().get() : nullptr;
}

const Entity* Scene::get_entity_from_uuid(UUID entity_uuid) const
{
    Optional<const OwnPtr<Entity>&> entity = m_entities.get_if_exists(entity_uuid);
    return entity.has_value() ? entity.value().get() : nullptr;
}

void Scene::set_primary_camera_entity(UUID entity_uuid)
{
    SE_ASSERT(entity_uuid != UUID::invalid());
    SE_ASSERT(m_entities.contains(entity_uuid));
    m_primary_camera_entity_uuid = entity_uuid;
}

UUID Scene::find_primary_camera_entity() const
{
    UUID primary_camera_entity_uuid = UUID::invalid();

    for (auto entity_it : m_entities)
    {
        const Entity& entity = *entity_it.value;
        if (entity.has_component<CameraComponent>())
        {
            const CameraComponent& cc = entity.get_component<CameraComponent>();
            if (cc.is_primary())
            {
                primary_camera_entity_uuid = entity.uuid();
                break;
            }
        }
    }

    return primary_camera_entity_uuid;
}

void Scene::on_begin_play()
{
    SE_ASSERT(m_play_state == PlayState::NotPlaying);
    m_play_state = PlayState::BeginPlaying;

    for (auto entity_it : m_entities)
    {
        entity_it.value->on_begin_play();
    }

    m_play_state = PlayState::Playing;
}

void Scene::on_end_play()
{
    SE_ASSERT(m_play_state == PlayState::Playing);
    m_play_state = PlayState::EndPlaying;

    for (auto entity_it : m_entities)
    {
        entity_it.value->on_end_play();
    }

    m_play_state = PlayState::NotPlaying;
}

void Scene::on_update(float delta_time)
{
    // Only allow calling `on_update` when the scene is actually playing.
    SE_ASSERT(m_play_state == PlayState::Playing);

    // Clear the primary camera entity UUID.
    // This entity is set by the `CameraComponent` during its `on_update` function.
    m_primary_camera_entity_uuid = UUID::invalid();

    for (auto entity_it : m_entities)
    {
        entity_it.value->on_update(delta_time);
    }
}

} // namespace SE
