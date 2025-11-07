/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Runtime/Scene/Forward.h>
#include <Runtime/Scene/Reflection/ReflectionComponent.h>
#include <Runtime/Scene/Reflection/ReflectionScript.h>

namespace SE
{

// Typedef of the regular 'UUID' utility structure for explicitly stating that the ID
// is meant to be used as an entity unique identifier.
using EntityID = UUID;

class Entity
{
public:
    ALWAYS_INLINE Entity()
        : m_EntityID(UUID::Invalid())
    {}

    ALWAYS_INLINE Entity(EntityID entityID, WeakRefPtr<Scene> scene)
        : m_EntityID(entityID)
        , m_SceneContext(Move(scene))
    {}

    Entity(const Entity&)     = default;
    Entity(Entity&&) noexcept = default;

    Entity& operator=(const Entity&)     = default;
    Entity& operator=(Entity&&) noexcept = default;

public:
    NODISCARD ALWAYS_INLINE bool IsValid() const { return (m_EntityID != UUID::Invalid()) && m_SceneContext.IsValid(); }
    NODISCARD ALWAYS_INLINE EntityID ID() const { return m_EntityID; }
    NODISCARD ALWAYS_INLINE WeakRefPtr<Scene> SceneContext() const { return m_SceneContext; }

    NODISCARD RUNTIME_API bool IsPendingDestroy() const;

public:
    NODISCARD RUNTIME_API bool HasComponent(UUID reflectionComponentUUID) const;

    NODISCARD RUNTIME_API Optional<void*> GetComponent(UUID reflectionComponentUUID);
    NODISCARD RUNTIME_API Optional<const void*> GetComponent(UUID reflectionComponentUUID) const;

    RUNTIME_API void* AddComponent(UUID reflectionComponentUUID);
    RUNTIME_API void RemoveComponent(UUID reflectionComponentUUID);

public:
    NODISCARD RUNTIME_API bool HasScript(UUID baseReflectionScriptUUID) const;

    NODISCARD RUNTIME_API Optional<Script&> GetScript(UUID baseReflectionScriptUUID);
    NODISCARD RUNTIME_API Optional<const Script&> GetScript(UUID baseReflectionScriptUUID) const;

    RUNTIME_API Script& AddScript(UUID baseReflectionScriptUUID);
    RUNTIME_API void RemoveScript(UUID baseReflectionScriptUUID);

public:
    template<typename ComponentType>
    NODISCARD ALWAYS_INLINE bool HasComponent() const
    {
        static_assert(ReflectionComponentMetadata<ComponentType>::IsSpecialized);
        const UUID reflectionComponentUUID = ReflectionComponentMetadata<ComponentType>::ComponentUUID;
        return HasComponent(reflectionComponentUUID);
    }

    template<typename ComponentType>
    NODISCARD ALWAYS_INLINE ComponentType& GetComponent()
    {
        static_assert(ReflectionComponentMetadata<ComponentType>::IsSpecialized);
        const UUID      reflectionComponentUUID = ReflectionComponentMetadata<ComponentType>::ComponentUUID;
        Optional<void*> componentMemoryBlock    = GetComponent(reflectionComponentUUID);
        SE_ASSERT(componentMemoryBlock.HasValue());
        return *static_cast<ComponentType*>(componentMemoryBlock.Value());
    }

    template<typename ComponentType>
    NODISCARD ALWAYS_INLINE const ComponentType& GetComponent() const
    {
        static_assert(ReflectionComponentMetadata<ComponentType>::IsSpecialized);
        const UUID            reflectionComponentUUID = ReflectionComponentMetadata<ComponentType>::ComponentUUID;
        Optional<const void*> componentMemoryBlock    = GetComponent(reflectionComponentUUID);
        SE_ASSERT(componentMemoryBlock.HasValue());
        return *static_cast<const ComponentType*>(componentMemoryBlock.Value());
    }

    template<typename ComponentType>
    ALWAYS_INLINE ComponentType& AddComponent()
    {
        static_assert(ReflectionComponentMetadata<ComponentType>::IsSpecialized);
        const UUID reflectionComponentUUID = ReflectionComponentMetadata<ComponentType>::ComponentUUID;
        void*      componentMemoryBlock    = AddComponent(reflectionComponentUUID);
        return *static_cast<ComponentType*>(componentMemoryBlock);
    }

    template<typename ComponentType>
    ALWAYS_INLINE void RemoveComponent()
    {
        static_assert(ReflectionComponentMetadata<ComponentType>::IsSpecialized);
        const UUID reflectionComponentUUID = ReflectionComponentMetadata<ComponentType>::ComponentUUID;
        RemoveComponent(reflectionComponentUUID);
    }

public:
    template<typename ScriptType>
    NODISCARD ALWAYS_INLINE bool HasScript() const
    {
        static_assert(ReflectionScriptMetadata<ScriptType>::IsSpecialized);
        const UUID reflectionScriptUUID = ReflectionScriptMetadata<ScriptType>::ScriptUUID;
        return HasScript(reflectionScriptUUID);
    }

    template<typename ScriptType>
    NODISCARD ALWAYS_INLINE ScriptType& GetScript()
    {
        static_assert(ReflectionScriptMetadata<ScriptType>::IsSpecialized);
        const UUID        reflectionScriptUUID = ReflectionScriptMetadata<ScriptType>::ScriptUUID;
        Optional<Script&> script               = GetScript(reflectionScriptUUID);
        SE_ASSERT(script.HasValue());
        return static_cast<ScriptType&>(script.Value());
    }

    template<typename ScriptType>
    NODISCARD ALWAYS_INLINE const ScriptType& GetScript() const
    {
        static_assert(ReflectionScriptMetadata<ScriptType>::IsSpecialized);
        const UUID              reflectionScriptUUID = ReflectionScriptMetadata<ScriptType>::ScriptUUID;
        Optional<const Script&> script               = GetScript(reflectionScriptUUID);
        SE_ASSERT(script.HasValue());
        return static_cast<const ScriptType&>(script.Value());
    }

    template<typename ScriptType>
    ALWAYS_INLINE ScriptType& AddScript()
    {
        static_assert(ReflectionScriptMetadata<ScriptType>::IsSpecialized);
        const UUID reflectionScriptUUID = ReflectionScriptMetadata<ScriptType>::ScriptUUID;
        return static_cast<ScriptType&>(AddScript(reflectionScriptUUID));
    }

    template<typename ScriptType>
    ALWAYS_INLINE void RemoveScript()
    {
        static_assert(ReflectionScriptMetadata<ScriptType>::IsSpecialized);
        const UUID reflectionScriptUUID = ReflectionScriptMetadata<ScriptType>::ScriptUUID;
        RemoveScript(reflectionScriptUUID);
    }

private:
    EntityID          m_EntityID;
    WeakRefPtr<Scene> m_SceneContext;
};

} // namespace SE
