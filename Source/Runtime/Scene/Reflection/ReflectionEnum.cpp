// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Scene/Reflection/ReflectionEnum.h>

namespace SE
{

void ReflectionEnum::SetEnumUUID(UUID enumUUID)
{
    SE_ASSERT(m_EnumUUID == UUID::Invalid());
    SE_ASSERT(enumUUID != UUID::Invalid());
    m_EnumUUID = enumUUID;
}

void ReflectionEnum::SetName(String name)
{
    SE_ASSERT(m_Name.IsEmpty());
    SE_ASSERT(name.HasCharacters());
    m_Name = Move(name);
}

void ReflectionEnum::SetUnderlyingDataType(PrimitiveDataType underlyingDataType)
{
    SE_ASSERT(m_UnderlyingDataType == PrimitiveDataType::Unknown);
    SE_ASSERT(underlyingDataType != PrimitiveDataType::Unknown);
    m_UnderlyingDataType = underlyingDataType;
}
    
void ReflectionEnum::AddMember(Member enumMember)
{
    SE_ASSERT(enumMember.Name.HasCharacters());
    m_Members.Add(Move(enumMember));
}

}
