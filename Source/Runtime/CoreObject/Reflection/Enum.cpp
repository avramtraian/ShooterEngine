// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/CoreObject/Reflection/Enum.h>

namespace SE
{

ObjectEnum::ObjectEnum(const ObjectInitializer& objectInitializer)
    : Object(objectInitializer)
{ 
}

void ObjectEnum::SetEnumName(String name)
{
    SE_ASSERT(m_EnumName.IsEmpty());
    m_EnumName = Move(name);
}

void ObjectEnum::SetUnderlyingPrimitiveType(PrimitiveType primitiveType)
{
    SE_ASSERT(m_UnderlyingPrimitiveType == PrimitiveType::Unknown);
    m_UnderlyingPrimitiveType = primitiveType;
}

void ObjectEnum::AddEnumMember(uint64 value, String name, Optional<String> description /*= {}*/)
{
    ObjectEnumMember& enumMember = m_EnumMembers.Emplace();
    enumMember.Value = value;
    enumMember.Name = Move(name);
    enumMember.Description = Move(description);
}

Optional<ObjectEnumMember> ObjectEnum::GetEnumMemberFromValue(uint64 value) const
{
    for (const ObjectEnumMember& enumMember : m_EnumMembers)
    {
        if (enumMember.Value == value)
            return enumMember;
    }

    return {};
}

Optional<ObjectEnumMember> ObjectEnum::GetEnumMemberFromName(StringView name) const
{
    for (const ObjectEnumMember& enumMember : m_EnumMembers)
    {
        if (enumMember.Name == name)
            return enumMember;
    }

    return {};
}

}
