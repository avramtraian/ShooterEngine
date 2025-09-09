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
    return m_Parents.Contains(baseObjectStruct);
}

void ObjectStruct::SetName(String name)
{
    SE_ASSERT(m_Name.IsEmpty());
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
    SE_ASSERT(!m_Parents.Contains(parent));
    SE_ASSERT(parent.IsValid());
    m_Parents.Add(Move(parent));
}

void ObjectStruct::AddField(const ObjectField& field)
{
    m_Fields.Add(field.Name, field);
}

void ObjectStruct::AddFieldDefaultValue(String fieldName, const ObjectFieldValue& defaultValue)
{
    SE_ASSERT(!m_FieldDefaultValues.Contains(fieldName));
    m_FieldDefaultValues.Add(Move(fieldName), defaultValue);
}

Optional<ObjectField> ObjectStruct::GetFieldFromName(StringView name) const
{
    const auto field = m_Fields.GetIfExists(name);
    if (field.HasValue())
        return field.Value();
    return {};
}

}
