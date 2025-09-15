// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Scene/Reflection/ReflectionScript.h>

namespace SE
{

void ReflectionScript::SetScriptUUID(UUID scriptUUID)
{
    SE_ASSERT(m_ScriptUUID == UUID::Invalid());
    SE_ASSERT(scriptUUID != UUID::Invalid());
    m_ScriptUUID = scriptUUID;
}

void ReflectionScript::SetParentScriptUUID(UUID parentScriptUUID)
{
    SE_ASSERT(m_ParentScriptUUID == UUID::Invalid());
    SE_ASSERT(parentScriptUUID != UUID::Invalid());
    m_ParentScriptUUID = parentScriptUUID;
}

void ReflectionScript::SetName(String name)
{
    SE_ASSERT(m_Name.IsEmpty());
    SE_ASSERT(name.HasCharacters());
    m_Name = Move(name);
}

void ReflectionScript::SetConstructFunction(PFN_ConstructScript function)
{
    SE_ASSERT(m_ConstructFunction == nullptr);
    SE_ASSERT(function != nullptr);
    m_ConstructFunction = function;
}

Script* ReflectionScript::ExecuteConstruct(void* dstMemoryBlock) const
{
    SE_ASSERT(m_ConstructFunction != nullptr);
    return m_ConstructFunction(dstMemoryBlock);
}

}
