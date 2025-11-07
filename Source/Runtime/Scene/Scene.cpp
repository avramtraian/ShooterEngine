/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Runtime/Core/Log.h>
#include <Runtime/Scene/Reflection/SceneReflectionRegistry.h>
#include <Runtime/Scene/Scene.h>

namespace SE
{

//========================================================================================================================================//
//------------------------------------------------------------ COMPONENT POOL ------------------------------------------------------------//
//========================================================================================================================================//

ComponentPool::ComponentPool(UUID reflectionComponentUUID, usize componentPoolCount)
{
    if (componentPoolCount == 0)
    {
        SE_LOG_ERROR("Trying to create a component pool with a initial size of zero (0).");
        SE_ASSERT_NOT_REACHED;
    }

    m_ReflectionComponent = SceneReflectionRegistry::GetComponentFromUUID(reflectionComponentUUID);

    // Allocate the pool memory buffer.
    const usize bufferByteCount = componentPoolCount * m_ReflectionComponent->GetStruct().GetStructureByteCount();
    m_ComponentsBuffer.SetByteCountWithoutCopy(bufferByteCount);

    // Initialize the empty byte offsets.
    m_EmptyByteOffsets.EnsureCapacity(componentPoolCount);
    for (usize byteOffset = 0; byteOffset < m_ComponentsBuffer.ByteCount(); byteOffset += m_ReflectionComponent->GetStruct().GetStructureByteCount())
        m_EmptyByteOffsets.Add(byteOffset);
}

bool ComponentPool::HasComponent(EntityID entityID) const
{
    SE_ASSERT(entityID != UUID::Invalid());
    const bool result = m_ByteOffsetTable.Contains(entityID);
    return result;
}

Optional<void*> ComponentPool::GetComponent(EntityID entityID)
{
    SE_ASSERT(entityID != UUID::Invalid());
    const Optional<usize> componentByteOffset = m_ByteOffsetTable.GetIfExists(entityID);
    if (!componentByteOffset.HasValue())
        return {};

    return static_cast<void*>(m_ComponentsBuffer.Bytes() + componentByteOffset.Value());
}

Optional<const void*> ComponentPool::GetComponent(EntityID entityID) const
{
    SE_ASSERT(entityID != UUID::Invalid());
    const Optional<usize> componentByteOffset = m_ByteOffsetTable.GetIfExists(entityID);
    if (!componentByteOffset.HasValue())
        return {};

    return static_cast<const void*>(m_ComponentsBuffer.Bytes() + componentByteOffset.Value());
}

void* ComponentPool::PushComponent(EntityID entityID)
{
    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(!m_ByteOffsetTable.Contains(entityID));

    if (m_EmptyByteOffsets.IsEmpty())
    {
        SE_LOG_ERROR("Overflown the component pool buffer!");
        SE_ASSERT_NOT_REACHED;
    }
    const usize componentByteOffset = m_EmptyByteOffsets.Last();
    m_EmptyByteOffsets.PopBack();
    m_ByteOffsetTable.Add(entityID, componentByteOffset);

    void* componentMemoryBlock = m_ComponentsBuffer.Bytes() + componentByteOffset;
    m_ReflectionComponent->ExecuteConstruct(componentMemoryBlock);
    return componentMemoryBlock;
}

void ComponentPool::RemoveComponent(EntityID entityID)
{
    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(m_ByteOffsetTable.Contains(entityID));
    const usize componentByteOffset  = m_ByteOffsetTable.At(entityID);
    void*       componentMemoryBlock = m_ComponentsBuffer.Bytes() + componentByteOffset;

    m_ReflectionComponent->ExecuteDestruct(componentMemoryBlock);
    m_ByteOffsetTable.RemoveUnchecked(entityID);
    m_EmptyByteOffsets.Add(componentByteOffset);
}

void ComponentPool::DefragmentAndSortBuffer()
{
    // TODO: Implement this routine! It should move the components around in the buffer such that:
    //         1) The components are tightly packed in a linear array.
    //         2) The components are sorted in ascending order of their corresponding parent entity UUID.
    //       The second condition ensures that memory access is efficient (from a caching perspective).
    //       These constraints don't have any effects on the functionality of the pool, and thus the defragment and sort
    //       operation are not guaranteed to be performed every frame, nor are they required to be performed in their entirety
    //       in a single frame (they can be performed across multiple frames).
}

//========================================================================================================================================//
//----------------------------------------------------------------- SCENE ----------------------------------------------------------------//
//========================================================================================================================================//

Scene::Scene()
    : m_SimulationState(SceneSimulationState::Stopped)
{
    // Create component pools for all registered component types.
    const auto registeredComponents = SceneReflectionRegistry::GetRegisteredComponents();
    for (const ReflectionComponent* reflectionComponent : registeredComponents)
    {
        // TODO: Let the user/programmer specify how big the initial buffer should be. The approach we currently have could be very wasteful.
        OwnPtr<ComponentPool> componentPool = CreateOwn<ComponentPool>(reflectionComponent->GetComponentUUID(), 128);
        m_ComponentPools.Add(reflectionComponent->GetComponentUUID(), Move(componentPool));
    }
}

Scene::~Scene()
{
    SE_ASSERT(m_SimulationState == SceneSimulationState::Stopped);
}

void Scene::OnBeginPlay()
{
    SE_ASSERT(m_SimulationState == SceneSimulationState::Stopped);
    m_SimulationState = SceneSimulationState::Simulating;

    // Call 'OnBeginPlay' for every script that is not pending destroy.
    for (auto& [entityID, entityMetadata] : m_Entities)
    {
        if (entityMetadata.LifecycleStage == EntityLifecycleStage::PendingBeginPlay)
        {
            entityMetadata.LifecycleStage = EntityLifecycleStage::Updating;
            for (auto& [reflectionScriptUUID, script] : entityMetadata.Scripts)
            {
                if (!script->IsPendingDestroy())
                    script->OnBeginPlay();
            }
        }
    }

    // Call 'OnBeginPlay' for every system.
    for (OwnPtr<System>& system : m_Systems)
        system->OnBeginPlay();
}

void Scene::OnEndPlay()
{
    SE_ASSERT(m_SimulationState == SceneSimulationState::Simulating);
    m_SimulationState = SceneSimulationState::Stopped;

    // Call 'OnEndPlay' for every script that is not pending destroy.
    for (auto& [entityID, entityMetadata] : m_Entities)
    {
        if (entityMetadata.LifecycleStage == EntityLifecycleStage::Updating)
        {
            entityMetadata.LifecycleStage = EntityLifecycleStage::PendingDestroy;
            for (auto& [reflectionScriptUUID, script] : entityMetadata.Scripts)
            {
                if (!script->IsPendingDestroy())
                    script->OnEndPlay();
            }
        }
    }

    // Call 'OnEndPlay' for every system.
    for (OwnPtr<System>& system : m_Systems)
        system->OnEndPlay();
}

void Scene::OnUpdate(float deltaTime)
{
    // NOTE: Update all scripts for every entity. It is important that updating scripts should happen before updating the systems,
    //       as that's the convention that we chose, and we should follow it strictly to ensure consistent behaviour.
    if (m_SimulationState == SceneSimulationState::Simulating)
    {
        for (auto& [entityID, entityMetadata] : m_Entities)
        {
            if (entityMetadata.LifecycleStage == EntityLifecycleStage::Updating)
            {
                for (auto& [reflectionScriptUUID, script] : entityMetadata.Scripts)
                {
                    if (script->GetLifecycleStage() == ScriptLifecycleStage::Updating)
                        script->OnUpdate(deltaTime);
                }
            }
        }
    }

    // Update all systems.
    for (OwnPtr<System>& system : m_Systems)
        system->OnUpdate(deltaTime);

    // NOTE: Execute deferred destructions. Is important that it is called only once we can be sure that no references to any
    //       entity, component or script are currently in use.
    ExecuteDeferredDestructions();
}

void Scene::PauseSimulation()
{}

void Scene::ResumeSimulation()
{}

Entity Scene::CreateEntity()
{
    const EntityID entityID = UUID::Generate();
    return CreateEntityWithUUID(entityID);
}

Entity Scene::CreateEntityWithUUID(EntityID entityID)
{
    // NOTE: Adding entities, components or scripts is not allowed when the scene is in the paused state.
    SE_ASSERT(m_SimulationState != SceneSimulationState::Paused);

    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(!m_Entities.Contains(entityID));

    EntityMetadata metadata = {};
    m_Entities.Add(entityID, Move(metadata));
    return GetEntityFromID(entityID);
}

void Scene::DestroyEntityDeferred(EntityID entityID)
{
    // NOTE: Calling the deferred version of the destroy entity function is only allowed when the scene is in the simulate state.
    //       Otherwise, the immediate version should be called.
    SE_ASSERT(m_SimulationState == SceneSimulationState::Simulating);

    SE_ASSERT(entityID != EntityID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    EntityMetadata& entityMetadata = m_Entities.At(entityID);
    SE_ASSERT(entityMetadata.LifecycleStage != EntityLifecycleStage::PendingDestroy);

    for (auto& [reflectionScriptUUID, script] : entityMetadata.Scripts)
    {
        if (entityMetadata.LifecycleStage == EntityLifecycleStage::Updating)
        {
            // Mark the script as pending destroy and call its 'OnEndPlay' method.
            SE_ASSERT(script->GetLifecycleStage() == ScriptLifecycleStage::Updating);
            script->OnEndPlay();
        }
    }

    m_PendingDestroyEntities.Add(entityID);
}

bool Scene::EntityIsPendingDestroy(EntityID entityID) const
{
    SE_ASSERT(entityID != EntityID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    return (m_Entities.At(entityID).LifecycleStage == EntityLifecycleStage::PendingDestroy);
}

bool Scene::EntityHasComponent(EntityID entityID, UUID reflectionComponentUUID) const
{
    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    SE_ASSERT(reflectionComponentUUID != UUID::Invalid());

    Optional<const OwnPtr<ComponentPool>&> componentPool = m_ComponentPools.GetIfExists(reflectionComponentUUID);
    SE_ASSERT(componentPool.HasValue());
    return componentPool.Value()->HasComponent(entityID);
}

Optional<void*> Scene::EntityGetComponent(EntityID entityID, UUID reflectionComponentUUID)
{
    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    SE_ASSERT(reflectionComponentUUID != UUID::Invalid());

    Optional<OwnPtr<ComponentPool>&> componentPool = m_ComponentPools.GetIfExists(reflectionComponentUUID);
    SE_ASSERT(componentPool.HasValue());
    return componentPool.Value()->GetComponent(entityID);
}

Optional<const void*> Scene::EntityGetComponent(EntityID entityID, UUID reflectionComponentUUID) const
{
    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    SE_ASSERT(reflectionComponentUUID != UUID::Invalid());

    Optional<const OwnPtr<ComponentPool>&> componentPool = m_ComponentPools.GetIfExists(reflectionComponentUUID);
    SE_ASSERT(componentPool.HasValue());
    return componentPool.Value()->GetComponent(entityID);
}

void* Scene::EntityAddComponent(EntityID entityID, UUID reflectionComponentUUID)
{
    // NOTE: Adding entities, components or scripts is not allowed when the scene is in the paused state.
    SE_ASSERT(m_SimulationState != SceneSimulationState::Paused);

    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    SE_ASSERT(reflectionComponentUUID != UUID::Invalid());

    Optional<OwnPtr<ComponentPool>&> componentPool = m_ComponentPools.GetIfExists(reflectionComponentUUID);
    SE_ASSERT(componentPool.HasValue());
    m_Entities.At(entityID).Components.Add(reflectionComponentUUID);
    return componentPool.Value()->PushComponent(entityID);
}

void Scene::EntityRemoveComponentDeferred(EntityID entityID, UUID reflectionComponentUUID)
{
    // Ensure that the entity ID and reflection component UUID are valid.
    SE_ASSERT(entityID != EntityID::Invalid());
    SE_ASSERT(reflectionComponentUUID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    SE_ASSERT(EntityHasComponent(entityID, reflectionComponentUUID));

    // NOTE: Unlike entities and scripts, the components don't have the concept of a pending-destroy state. This means that
    //       there is no flag that needs to be set in this function, and that the user can't check whether a component will
    //       be destroyed at the end of the frame.

    // Push an entry to the pending-destroy component list.
    PendingDestroyComponentEntry& entry = m_PendingDestroyComponents.Emplace();
    entry.Entity                        = entityID;
    entry.ReflectionComponentUUID       = reflectionComponentUUID;
}

bool Scene::EntityHasScript(EntityID entityID, UUID baseReflectionScriptUUID) const
{
    const Optional<UUID> reflectionScriptUUID = EntityGetReflectionScriptUUIDFromBase(entityID, baseReflectionScriptUUID);
    return reflectionScriptUUID.HasValue();
}

Optional<Script&> Scene::EntityGetScript(EntityID entityID, UUID baseReflectionScriptUUID)
{
    const Optional<UUID> reflectionScriptUUID = EntityGetReflectionScriptUUIDFromBase(entityID, baseReflectionScriptUUID);
    if (!reflectionScriptUUID.HasValue())
        return {};

    return *m_Entities.At(entityID).Scripts.At(reflectionScriptUUID.Value());
}

Optional<const Script&> Scene::EntityGetScript(EntityID entityID, UUID baseReflectionScriptUUID) const
{
    const Optional<UUID> reflectionScriptUUID = EntityGetReflectionScriptUUIDFromBase(entityID, baseReflectionScriptUUID);
    if (!reflectionScriptUUID.HasValue())
        return {};

    return *m_Entities.At(entityID).Scripts.At(reflectionScriptUUID.Value());
}

Script& Scene::EntityAddScript(EntityID entityID, UUID reflectionScriptUUID)
{
    // NOTE: Adding entities, components or scripts is not allowed when the scene is in the paused state.
    SE_ASSERT(m_SimulationState != SceneSimulationState::Paused);

    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(reflectionScriptUUID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    auto& entityMetadata = m_Entities.At(entityID);

    if (EntityHasScript(entityID, reflectionScriptUUID))
    {
        SE_LOG_ERROR("Trying to add a script to an entity that already has a script derived from that type!");
        SE_ASSERT_NOT_REACHED;
    }

    // TODO: Currently, allocating scripts is very wasteful. Maybe implement an arena system similar to how components are allocated.
    //       Any other allocation strategy would be more efficient, more performant and produce less fragmentation then what we currently
    //       do (allocating each script using the default 'new' heap allocator).

    ScriptInitializationInfo scriptInitializationInfo = {};
    scriptInitializationInfo.ReflectionStructUUID     = reflectionScriptUUID;
    scriptInitializationInfo.EntityID                 = entityID;
    scriptInitializationInfo.SceneContext             = AdoptStrongRef(this);

    const ReflectionScript& reflectionScript  = SceneReflectionRegistry::GetScriptFromUUID(reflectionScriptUUID);
    void*                   scriptMemoryBlock = ::operator new(reflectionScript.GetStruct().GetStructureByteCount());
    Script*                 rawScript         = reflectionScript.ExecuteConstruct(scriptMemoryBlock, scriptInitializationInfo);
    OwnPtr<Script>&         script            = entityMetadata.Scripts.Add(reflectionScriptUUID, AdoptOwn(rawScript));

    if (m_SimulationState == SceneSimulationState::Simulating)
        script->OnBeginPlay();

    return *script.Get();
}

void Scene::EntityRemoveScriptDeferred(EntityID entityID, UUID baseReflectionScriptUUID)
{
    // NOTE: Calling the deferred version of the destroy entity function is only allowed when the scene is in the simulate state.
    //       Otherwise, the immediate version should be called.
    SE_ASSERT(m_SimulationState == SceneSimulationState::Simulating);

    // Ensure that the entity ID and reflection script UUID are valid.
    const Optional<UUID> reflectionScriptUUID = EntityGetReflectionScriptUUIDFromBase(entityID, baseReflectionScriptUUID);
    SE_ASSERT(reflectionScriptUUID.HasValue());

    // Mark the script as pending destroy and call its 'OnEndPlay' method.
    OwnPtr<Script>& script = m_Entities.At(entityID).Scripts.At(reflectionScriptUUID.Value());
    SE_ASSERT(script->GetLifecycleStage() == ScriptLifecycleStage::Updating);
    script->OnEndPlay();

    // Push an entry to the pending-destroy script list.
    PendingDestroyScriptEntry& entry = m_PendingDestroyScripts.Emplace();
    entry.Entity                     = entityID;
    entry.ReflectionScriptUUID       = baseReflectionScriptUUID;
}

void Scene::DestroyEntityImmediately(EntityID entityID)
{
    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));

    EntityMetadata& metadata = m_Entities.At(entityID);
    for (UUID reflectionComponentUUID : metadata.Components)
    {
        Optional<OwnPtr<ComponentPool>&> componentPool = m_ComponentPools.GetIfExists(reflectionComponentUUID);
        SE_ASSERT(componentPool.HasValue());
        componentPool.Value()->RemoveComponent(entityID);
    }

    m_Entities.RemoveUnchecked(entityID);
}

void Scene::EntityRemoveComponentImmediately(EntityID entityID, UUID reflectionComponentUUID)
{
    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    SE_ASSERT(reflectionComponentUUID != UUID::Invalid());
    auto& entityMetadata = m_Entities.At(entityID);
    SE_ASSERT(entityMetadata.Components.Contains(reflectionComponentUUID));

    Optional<OwnPtr<ComponentPool>&> componentPool = m_ComponentPools.GetIfExists(reflectionComponentUUID);
    SE_ASSERT(componentPool.HasValue());
    componentPool.Value()->RemoveComponent(entityID);
    entityMetadata.Components.RemoveUnchecked(reflectionComponentUUID);
}

void Scene::EntityRemoveScriptImmediately(EntityID entityID, UUID baseReflectionScriptUUID)
{
    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(baseReflectionScriptUUID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    auto& entityMetadata = m_Entities.At(entityID);

    const Optional<UUID> removeReflectionScriptUUID = EntityGetReflectionScriptUUIDFromBase(entityID, baseReflectionScriptUUID);
    if (!removeReflectionScriptUUID.HasValue())
    {
        SE_LOG_ERROR("Trying to remove a script from an entity that has no script derived from that base type!");
        SE_ASSERT_NOT_REACHED;
    }

    entityMetadata.Scripts.RemoveUnchecked(removeReflectionScriptUUID.Value());
}

void Scene::ExecuteDeferredDestructions()
{
    // Destroy entities.
    for (EntityID entityID : m_PendingDestroyEntities)
    {
        if (m_Entities.Contains(entityID))
            DestroyEntityImmediately(entityID);
    }
    m_PendingDestroyEntities.Clear();

    // Destroy components.
    for (const PendingDestroyComponentEntry& entry : m_PendingDestroyComponents)
    {
        const auto& entityMetadata = m_Entities.GetIfExists(entry.Entity);
        if (entityMetadata.HasValue() && entityMetadata->Components.Contains(entry.ReflectionComponentUUID))
            EntityRemoveComponentImmediately(entry.Entity, entry.ReflectionComponentUUID);
    }
    m_PendingDestroyComponents.Clear();

    // Destroy scripts.
    for (const PendingDestroyScriptEntry& entry : m_PendingDestroyScripts)
    {
        const auto& entityMetadata = m_Entities.GetIfExists(entry.Entity);
        if (entityMetadata.HasValue() && entityMetadata->Scripts.Contains(entry.ReflectionScriptUUID))
            EntityRemoveScriptImmediately(entry.Entity, entry.ReflectionScriptUUID);
    }
    m_PendingDestroyScripts.Clear();
}

Optional<UUID> Scene::EntityGetReflectionScriptUUIDFromBase(EntityID entityID, UUID baseReflectionScriptUUID) const
{
    SE_ASSERT(entityID != UUID::Invalid());
    SE_ASSERT(baseReflectionScriptUUID != UUID::Invalid());
    SE_ASSERT(m_Entities.Contains(entityID));
    const auto& entityMetadata = m_Entities.At(entityID);

    for (const auto& [reflectionScriptUUID, script] : entityMetadata.Scripts)
    {
        if (script->IsDerivedFrom(baseReflectionScriptUUID))
            return reflectionScriptUUID;
    }

    // The entity has no script that is derived from the base script with the given reflection UUID.
    return {};
}

} // namespace SE
