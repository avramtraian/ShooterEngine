// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/CoreObject/Reflection/Struct.h>

namespace SE
{

ObjectStruct::ObjectStruct(const ObjectInitializer& objectInitializer)
    : Object(objectInitializer)
{
}

bool ObjectStruct::IsDerivedFrom(SObjectPtr<ObjectStruct> baseObjectStruct) const
{
    if (baseObjectStruct.Get() == this)
        return true;

    const auto& parentChain = GetParentChain();
    return parentChain.Contains(baseObjectStruct);
}

const HashSet<SObjectPtr<ObjectStruct>>& ObjectStruct::GetParentChain() const
{
    if (m_CachedParentChain.IsEmpty())
    {
        if (m_Parent.IsValid())
        {
            // Add the parents of the parent.
            m_CachedParentChain = m_Parent->GetParentChain();
        
            // Add the direct parent.
            m_CachedParentChain.Add(m_Parent);
        }
    }
    return m_CachedParentChain;
}

const HashMap<String, ObjectField>& ObjectStruct::GetFields() const
{
    if (m_CachedFields.IsEmpty())
    {
        // Add inherited fields, if the structure is derived from something.
        if (m_Parent.IsValid())
            m_CachedFields = m_Parent->GetFields();

        // Add the non-inherited fields to the list.
        for (const auto& [fieldName, nonInheritedField] : m_NonInheritedFields)
            m_CachedFields.Add(fieldName, nonInheritedField);
    }
    return m_CachedFields;
}

void ObjectStruct::SetName(String name)
{
    SE_ASSERT(m_Name.IsEmpty());
    SE_ASSERT(name.HasCharacters());
    m_Name = Move(name);
}

void ObjectStruct::SetStructureByteCount(usize structureByteCount)
{
    SE_ASSERT(m_StructureByteCount == 0);
    SE_ASSERT(structureByteCount > 0);
    m_StructureByteCount = structureByteCount;
}

void ObjectStruct::SetParent(SObjectPtr<ObjectStruct> parent)
{
    SE_ASSERT(!m_Parent.IsValid());
    SE_ASSERT(parent.IsValid());
    m_Parent = Move(parent);
}

void ObjectStruct::AddNonInheritedField(const ObjectField& field)
{
    m_NonInheritedFields.Add(field.Name, field);
}

void ObjectStruct::AddFieldDefaultValue(String fieldName, const ObjectFieldValue& defaultValue)
{
    SE_ASSERT(!m_FieldDefaultValues.Contains(fieldName));
    m_FieldDefaultValues.Add(Move(fieldName), defaultValue);
}

}
