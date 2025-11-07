/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Runtime/Scene/Entity.h>
#include <Runtime/Scene/Scene.h>

namespace SE
{

bool Entity::IsPendingDestroy() const
{
    SE_ASSERT(IsValid());
    return m_SceneContext->EntityIsPendingDestroy(m_EntityID);
}

bool Entity::HasComponent(UUID reflectionComponentUUID) const
{
    if (!IsValid())
        return false;
    return m_SceneContext->EntityHasComponent(m_EntityID, reflectionComponentUUID);
}

Optional<void*> Entity::GetComponent(UUID reflectionComponentUUID)
{
    if (!IsValid())
        return {};
    return m_SceneContext->EntityGetComponent(m_EntityID, reflectionComponentUUID);
}

Optional<const void*> Entity::GetComponent(UUID reflectionComponentUUID) const
{
    if (!IsValid())
        return {};
    return m_SceneContext->EntityGetComponent(m_EntityID, reflectionComponentUUID);
}

void* Entity::AddComponent(UUID reflectionComponentUUID)
{
    SE_ASSERT(IsValid());
    return m_SceneContext->EntityAddComponent(m_EntityID, reflectionComponentUUID);
}

void Entity::RemoveComponent(UUID reflectionComponentUUID)
{
    SE_ASSERT(IsValid());
    m_SceneContext->EntityRemoveComponentDeferred(m_EntityID, reflectionComponentUUID);
}

bool Entity::HasScript(UUID baseReflectionScriptUUID) const
{
    if (!IsValid())
        return false;
    return m_SceneContext->EntityHasScript(m_EntityID, baseReflectionScriptUUID);
}

Optional<Script&> Entity::GetScript(UUID baseReflectionScriptUUID)
{
    if (!IsValid())
        return {};
    return m_SceneContext->EntityGetScript(m_EntityID, baseReflectionScriptUUID);
}

Optional<const Script&> Entity::GetScript(UUID baseReflectionScriptUUID) const
{
    if (!IsValid())
        return {};
    return m_SceneContext->EntityGetScript(m_EntityID, baseReflectionScriptUUID);
}

Script& Entity::AddScript(UUID baseReflectionScriptUUID)
{
    SE_ASSERT(IsValid());
    return m_SceneContext->EntityAddScript(m_EntityID, baseReflectionScriptUUID);
}

void Entity::RemoveScript(UUID baseReflectionScriptUUID)
{
    SE_ASSERT(IsValid());
    m_SceneContext->EntityRemoveScriptDeferred(m_EntityID, baseReflectionScriptUUID);
}

} // namespace SE
