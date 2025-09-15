// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Scene/Reflection/ReflectionSystem.h>

namespace SE
{

void ReflectionSystem::SetSystemUUID(UUID systemUUID)
{
    SE_ASSERT(m_SystemUUID == UUID::Invalid());
    SE_ASSERT(systemUUID != UUID::Invalid());
    m_SystemUUID = systemUUID;
}
    
void ReflectionSystem::SetName(String name)
{
    SE_ASSERT(m_Name.IsEmpty());
    SE_ASSERT(name.HasCharacters());
    m_Name = Move(name);
}

void ReflectionSystem::SetConstructFunction(PFN_ConstructSystem function)
{
    SE_ASSERT(m_ConstructFunction == nullptr);
    SE_ASSERT(function != nullptr);
    m_ConstructFunction = function;
}

System* ReflectionSystem::ExecuteConstruct(void* dstMemoryBlock)
{
    SE_ASSERT(m_ConstructFunction != nullptr);
    return m_ConstructFunction(dstMemoryBlock);
}

}
