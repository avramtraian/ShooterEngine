// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Scene/Reflection/ReflectionComponent.h>

namespace SE
{

void ReflectionComponent::SetComponentUUID(UUID componentUUID)
{
    SE_ASSERT(m_ComponentUUID == UUID::Invalid());
    SE_ASSERT(componentUUID != UUID::Invalid());
    m_ComponentUUID = componentUUID;
}

void ReflectionComponent::SetName(String name)
{
    SE_ASSERT(m_Name.IsEmpty());
    SE_ASSERT(name.HasCharacters());
    m_Name = Move(name);
}

void ReflectionComponent::SetConstructFunction(PFN_ConstructComponent function)
{
    SE_ASSERT(m_ConstructFunction == nullptr);
    SE_ASSERT(function != nullptr);
    m_ConstructFunction = function;
}

void ReflectionComponent::SetDestructFunction(PFN_DestructComponent function)
{
    SE_ASSERT(m_DestructFunction == nullptr);
    SE_ASSERT(function != nullptr);
    m_DestructFunction = function;
}

void ReflectionComponent::SetCopyFunction(PFN_CopyComponent function)
{
    SE_ASSERT(m_CopyFunction == nullptr);
    SE_ASSERT(function != nullptr);
    m_CopyFunction = function;
}

void ReflectionComponent::ExecuteConstruct(void* dstMemoryBlock)
{
    SE_ASSERT(m_ConstructFunction != nullptr);
    m_ConstructFunction(dstMemoryBlock);
}

void ReflectionComponent::ExecuteDestruct(void* dstMemoryBlock)
{
    SE_ASSERT(m_DestructFunction != nullptr);
    m_DestructFunction(dstMemoryBlock);
}

void ReflectionComponent::ExecuteCopy(void* dstMemoryBlock, const void* srcMemoryBlock)
{
    SE_ASSERT(m_CopyFunction != nullptr);
    m_CopyFunction(dstMemoryBlock, srcMemoryBlock);
}

}
