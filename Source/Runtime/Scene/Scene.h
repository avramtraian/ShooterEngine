/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Core/Containers/RefPtr.h>
#include <Runtime/Core/Memory/Buffer.h>
#include <Runtime/Scene/Entity.h>
#include <Runtime/Scene/Reflection/ReflectionComponent.h>
#include <Runtime/Scene/Script.h>
#include <Runtime/Scene/System.h>

namespace SE
{

//========================================================================================================================================//
//------------------------------------------------------------ COMPONENT POOL ------------------------------------------------------------//
//========================================================================================================================================//

class ComponentPool
{
    SE_MAKE_NONCOPYABLE(ComponentPool);
    SE_MAKE_NONMOVABLE(ComponentPool);

public:
    RUNTIME_API ComponentPool(UUID reflectionComponentUUID, usize componentPoolCount);

    NODISCARD RUNTIME_API bool HasComponent(EntityID entityID) const;
    NODISCARD RUNTIME_API Optional<void*> GetComponent(EntityID entityID);
    NODISCARD RUNTIME_API Optional<const void*> GetComponent(EntityID entityID) const;

    // NOTE: These functions do not modify any existing pointers/references to other components stored in the pool, so they are safe to call
    // inside gameplay loops. While they do create memory fragmentation inside the buffer, at the end of each frame the 'DefragmentBuffer'
    // is called (which *does* invalidate pointers/references).
    RUNTIME_API void* PushComponent(EntityID entityID);
    RUNTIME_API void RemoveComponent(EntityID entityID);

    // NOTE: Expect all pointers/references of the components stored inside this pool to be invalidated after calling this
    //       method. This means that it should not be called inside gameplay loops.
    RUNTIME_API void DefragmentAndSortBuffer();

private:
    Optional<const ReflectionComponent&> m_ReflectionComponent;
    Buffer                               m_ComponentsBuffer;
    HashMap<UUID, usize>                 m_ByteOffsetTable;
    Vector<usize>                        m_EmptyByteOffsets;
};

//========================================================================================================================================//
//----------------------------------------------------------------- SCENE ----------------------------------------------------------------//
//========================================================================================================================================//

enum class SceneSimulationState : uint8
{
    Stopped,
    Simulating,
    Paused,
};

class Scene : public RefCounted
{
    SE_MAKE_NONCOPYABLE(Scene);
    SE_MAKE_NONMOVABLE(Scene);

public:
    enum class EntityLifecycleStage : uint8
    {
        PendingBeginPlay,
        Updating,
        PendingDestroy,
    };

    struct EntityMetadata
    {
        HashSet<UUID>                 Components;
        HashMap<UUID, OwnPtr<Script>> Scripts;
        EntityLifecycleStage          LifecycleStage = EntityLifecycleStage::PendingBeginPlay;
    };

public:
    RUNTIME_API Scene();
    RUNTIME_API ~Scene();

    RUNTIME_API void OnBeginPlay();
    RUNTIME_API void OnEndPlay();
    RUNTIME_API void OnUpdate(float deltaTime);

    NODISCARD ALWAYS_INLINE bool IsSimulating() const { return (m_SimulationState == SceneSimulationState::Simulating); }
    RUNTIME_API void PauseSimulation();
    RUNTIME_API void ResumeSimulation();

public:
    RUNTIME_API Entity CreateEntity();
    RUNTIME_API Entity CreateEntityWithUUID(EntityID entityID);

    RUNTIME_API void DestroyEntityDeferred(EntityID entityID);

    // Utility functions for creating an entity utility object from an ID.
    NODISCARD ALWAYS_INLINE Entity GetEntityFromID(EntityID entityID) { return Entity(entityID, AdoptWeakRef(this)); }

    // Returns a list of all currently used entity IDs, no matter the lifecycle of the entity.
    // TODO: This is very inefficient from a memory usage perspective. This should be an iterator over the internal hash map
    //       of entities instead of allocating a memory block just to store the entities IDs.
    NODISCARD RUNTIME_API Vector<EntityID> QueryAllEntities() const;

public:
    NODISCARD RUNTIME_API bool EntityIsPendingDestroy(EntityID entityID) const;

    NODISCARD RUNTIME_API bool EntityHasComponent(EntityID entityID, UUID reflectionComponentUUID) const;
    NODISCARD RUNTIME_API Optional<void*> EntityGetComponent(EntityID entityID, UUID reflectionComponentUUID);
    NODISCARD RUNTIME_API Optional<const void*> EntityGetComponent(EntityID entityID, UUID reflectionComponentUUID) const;

    RUNTIME_API void* EntityAddComponent(EntityID entityID, UUID reflectionComponentUUID);
    RUNTIME_API void EntityRemoveComponentDeferred(EntityID entityID, UUID reflectionComponentUUID);

public:
    NODISCARD RUNTIME_API bool EntityHasScript(EntityID entityID, UUID baseReflectionScriptUUID) const;
    NODISCARD RUNTIME_API Optional<Script&> EntityGetScript(EntityID entityID, UUID baseReflectionScriptUUID);
    NODISCARD RUNTIME_API Optional<const Script&> EntityGetScript(EntityID entityID, UUID baseReflectionScriptUUID) const;

    RUNTIME_API Script& EntityAddScript(EntityID entityID, UUID reflectionScriptUUID);
    RUNTIME_API void EntityRemoveScriptDeferred(EntityID entityID, UUID baseReflectionScriptUUID);

public:
    RUNTIME_API void DestroyEntityImmediately(EntityID entityID);
    RUNTIME_API void EntityRemoveComponentImmediately(EntityID entityID, UUID reflectionComponentUUID);
    RUNTIME_API void EntityRemoveScriptImmediately(EntityID entityID, UUID baseReflectionScriptUUID);

private:
    void ExecuteDeferredDestructions();

    NODISCARD Optional<UUID> EntityGetReflectionScriptUUIDFromBase(EntityID entityID, UUID baseReflectionScriptUUID) const;

private:
    HashMap<UUID, EntityMetadata>        m_Entities;
    HashMap<UUID, OwnPtr<ComponentPool>> m_ComponentPools;
    Vector<OwnPtr<System>>               m_Systems;
    SceneSimulationState                 m_SimulationState;

    struct PendingDestroyComponentEntry
    {
        EntityID Entity;
        UUID     ReflectionComponentUUID;
    };

    struct PendingDestroyScriptEntry
    {
        EntityID Entity;
        UUID     ReflectionScriptUUID;
    };

    Vector<EntityID>                     m_PendingDestroyEntities;
    Vector<PendingDestroyComponentEntry> m_PendingDestroyComponents;
    Vector<PendingDestroyScriptEntry>    m_PendingDestroyScripts;
};

} // namespace SE
