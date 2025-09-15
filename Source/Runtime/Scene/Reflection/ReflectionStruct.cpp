// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Scene/Reflection/ReflectionStruct.h>

namespace SE
{

void ReflectionStruct::SetStructUUID(UUID structUUID)
{
    SE_ASSERT(m_StructUUID == UUID::Invalid());
    SE_ASSERT(structUUID != UUID::Invalid());
    m_StructUUID = structUUID;
}

void ReflectionStruct::SetParentStructUUID(UUID parentStructUUID)
{
    SE_ASSERT(m_ParentStructUUID == UUID::Invalid());
    SE_ASSERT(parentStructUUID != UUID::Invalid());
    m_ParentStructUUID = parentStructUUID;
}

void ReflectionStruct::SetName(String name)
{
    SE_ASSERT(m_Name.IsEmpty());
    SE_ASSERT(name.HasCharacters());
    m_Name = Move(name);
}

void ReflectionStruct::SetStructureByteCount(usize structureByteCount)
{
    SE_ASSERT(m_StructureByteCount == 0);
    SE_ASSERT(structureByteCount > 0);
    m_StructureByteCount = structureByteCount;
}

void ReflectionStruct::AddNonInheritedField(Field field)
{
    m_NonInheritedFields.Add(Move(field));
}

void ReflectionStruct::SetConstructFunction(PFN_ConstructStruct function)
{
    SE_ASSERT(m_ConstructFunction == nullptr);
    SE_ASSERT(function != nullptr);
    m_ConstructFunction = function;
}

void ReflectionStruct::SetDestructFunction(PFN_DestructStruct function)
{
    SE_ASSERT(m_DestructFunction == nullptr);
    SE_ASSERT(function != nullptr);
    m_DestructFunction = function;
}

void ReflectionStruct::SetCopyFunction(PFN_CopyStruct function)
{
    SE_ASSERT(m_CopyFunction == nullptr);
    SE_ASSERT(function != nullptr);
    m_CopyFunction = function;
}

void ReflectionStruct::ExecuteConstruct(void* dstMemoryBlock)
{
    SE_ASSERT(m_ConstructFunction != nullptr);
    m_ConstructFunction(dstMemoryBlock);
}

void ReflectionStruct::ExecuteDestruct(void* dstMemoryBlock)
{
    SE_ASSERT(m_DestructFunction != nullptr);
    m_DestructFunction(dstMemoryBlock);
}

void ReflectionStruct::ExecuteCopy(void* dstMemoryBlock, const void* srcMemoryBlock)
{
    SE_ASSERT(m_CopyFunction != nullptr);
    m_CopyFunction(dstMemoryBlock, srcMemoryBlock);
}

void* ReflectionStruct::AllocateFieldType(usize fieldTypeByteCount)
{
    // TODO(Traian): Allocate the required memory from a linear arena instead of directly from the
    // heap. Ignoring the performance cost, this memory block will never be reclaimed. While for now
    // this is not a memory leak (since the reflection structs persist for the entire lifetime of the
    // application), when we implement hot-reloading for game code modules this will be problematic.
    return ::operator new(fieldTypeByteCount);
}

}
