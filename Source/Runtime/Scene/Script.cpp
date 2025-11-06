// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Scene/Reflection/SceneReflectionRegistry.h>
#include <Runtime/Scene/Script.h>

namespace SE
{

Script::Script(UUID reflectionScriptUUID)
    : m_ReflectionScriptUUID(reflectionScriptUUID)
{}

Script::~Script()
{}

bool Script::IsDerivedFrom(UUID baseReflectionScriptUUID) const
{
    const ReflectionScript& reflectionScript = SceneReflectionRegistry::GetScriptFromUUID(m_ReflectionScriptUUID);
    return reflectionScript.GetParentHierarchiyUUIDs().Contains(baseReflectionScriptUUID);
}

void Script::OnBeginPlay()
{}

void Script::OnEndPlay()
{}

void Script::OnUpdate(float deltaTime)
{}

} // namespace SE
