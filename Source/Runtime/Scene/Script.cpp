// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Scene/Reflection/SceneReflectionRegistry.h>
#include <Runtime/Scene/Scene.h>
#include <Runtime/Scene/Script.h>

namespace SE
{

Script::Script(const ScriptInitializationInfo& initializationInfo)
    : m_ReflectionScriptUUID(initializationInfo.ReflectionStructUUID)
    , m_LifecycleStage(ScriptLifecycleStage::PendingBeginPlay)
{
    SE_ASSERT(initializationInfo.SceneContext.IsValid());
    m_ParentEntity = initializationInfo.SceneContext.GetNonConst()->GetEntityFromID(initializationInfo.EntityID);
}

Script::~Script()
{
    SE_ASSERT(IsPendingDestroy());
}

bool Script::IsDerivedFrom(UUID baseReflectionScriptUUID) const
{
    const ReflectionScript& reflectionScript = SceneReflectionRegistry::GetScriptFromUUID(m_ReflectionScriptUUID);
    return reflectionScript.GetParentHierarchiyUUIDs().Contains(baseReflectionScriptUUID);
}

void Script::OnBeginPlay()
{
    SE_ASSERT(m_LifecycleStage == ScriptLifecycleStage::PendingBeginPlay);
    m_LifecycleStage = ScriptLifecycleStage::Updating;
}

void Script::OnEndPlay()
{
    SE_ASSERT(m_LifecycleStage == ScriptLifecycleStage::Updating);
    m_LifecycleStage = ScriptLifecycleStage::PendingDestroy;
}

void Script::OnUpdate(float deltaTime)
{
    SE_ASSERT(m_LifecycleStage == ScriptLifecycleStage::Updating);
}

} // namespace SE
