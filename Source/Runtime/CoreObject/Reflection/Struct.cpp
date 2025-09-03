// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/CoreObject/Reflection/Struct.h>

namespace SE
{

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
    SE_ASSERT(!m_Parent.IsValid());
    SE_ASSERT(parent.IsValid());
    m_Parent = Move(parent);
}

void ObjectStruct::AddField(const ObjectField& field)
{
    m_Fields.Add(field);
}

Optional<ObjectField> ObjectStruct::GetFieldFromName(StringView name) const
{
    for (const ObjectField& field : m_Fields)
    {
        if (field.Name == name)
            return field;
    }

    return {};
}

}
