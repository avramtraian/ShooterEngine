/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/HashMap.h>
#include <Core/Containers/OwnPtr.h>
#include <Core/Misc/IterationDecision.h>
#include <Engine/Scene/Entity.h>
#include <Engine/Scene/EntityComponent.h>

namespace SE
{

//
// The scene class is responsible for managing a collection of entities.
//
class Scene
{
    SE_MAKE_NONCOPYABLE(Scene);
    SE_MAKE_NONMOVABLE(Scene);

    friend class OwnPtr<Scene>;

public:
    enum class PlayState : u8
    {
        NotPlaying,
        BeginPlaying,
        Playing,
        EndPlaying,
    };

public:
    NODISCARD SHOOTER_API static OwnPtr<Scene> create();

public:
    // Returns the current play state of the scene.
    NODISCARD ALWAYS_INLINE PlayState get_play_state() const { return m_play_state; }

    // Returns the number of active entities in the scene.
    NODISCARD ALWAYS_INLINE u32 get_entity_count() const { return static_cast<u32>(m_entities.count()); }

public:
    SHOOTER_API Entity* create_entity();
    SHOOTER_API Entity* create_entity_with_uuid(UUID entity_uuid);

    // These functions return null pointers if no entity has the given UUID.
    SHOOTER_API Entity* get_entity_from_uuid(UUID entity_uuid);
    SHOOTER_API const Entity* get_entity_from_uuid(UUID entity_uuid) const;

    SHOOTER_API void set_primary_camera_entity(UUID entity_uuid);
    NODISCARD ALWAYS_INLINE UUID get_primary_camera_entity_uuid() const { return m_primary_camera_entity_uuid; }

    NODISCARD ALWAYS_INLINE Entity* get_primary_camera_entity() { return get_entity_from_uuid(m_primary_camera_entity_uuid); }
    NODISCARD ALWAYS_INLINE const Entity* get_primary_camera_entity() const { return get_entity_from_uuid(m_primary_camera_entity_uuid); }

    // Iterates over all entities and returns the UUID of the first entity that has a camera component marked as primary.
    // Returns UUID::invalid() if no primary camera entity exists.
    NODISCARD SHOOTER_API UUID find_primary_camera_entity() const;

public:
    //
    // The signature of the provided entity predicate function must:
    //   - Return `IterationDecision`, which determines whether or not to finish the entity iteration process;
    //   - Have the following parameters (Entity* entity, UUID entity_uuid).
    //
    template<typename EntityPredicate>
    ALWAYS_INLINE void for_each_entity(EntityPredicate entity_predicate)
    {
        for (auto entity_uuid_pair : m_entities)
        {
            UUID entity_uuid = entity_uuid_pair.key;
            OwnPtr<Entity>& entity = entity_uuid_pair.value;

            const IterationDecision iteration_decision = entity_predicate(entity.get(), entity_uuid);
            if (iteration_decision == IterationDecision::Break)
                break;
        }
    }

    //
    // The signature of the provided entity predicate function must:
    //   - Return `IterationDecision`, which determines whether or not to finish the entity iteration process;
    //   - Have the following parameters (const Entity* entity, UUID entity_uuid).
    //
    template<typename EntityPredicate>
    ALWAYS_INLINE void for_each_entity(EntityPredicate entity_predicate) const
    {
        for (const auto entity_uuid_pair : m_entities)
        {
            UUID entity_uuid = entity_uuid_pair.key;
            const OwnPtr<Entity>& entity = entity_uuid_pair.value;

            const IterationDecision iteration_decision = entity_predicate(entity.get(), entity_uuid);
            if (iteration_decision == IterationDecision::Break)
                break;
        }
    }

public:
    //
    // Invokes the `on_begin_play` callback for each entity in the scene.
    // Also runs the initialization logic required for the scene to start playing a new session.
    //
    SHOOTER_API void on_begin_play();

    //
    // Invokes the `on_end_play` callback for each entity in the scene.
    // Also runs the logic that is required for the scene to correctly end the current playing session.
    //
    SHOOTER_API void on_end_play();

    //
    // Invokes the `on_update` callback for each entity in the scene.
    // Also runs the update logic required for the current play session.
    //
    SHOOTER_API void on_update(float delta_time);

private:
    SHOOTER_API Scene();
    SHOOTER_API ~Scene();

private:
    PlayState m_play_state;

    HashMap<UUID, OwnPtr<Entity>> m_entities;

    // The UUID of the enyity that has a camera component attached to it and it is also
    // marked as the primary one.
    UUID m_primary_camera_entity_uuid;
};

} // namespace SE
