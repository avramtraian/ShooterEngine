// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/UUID.h>
#include <Runtime/Scene/Reflection/PrimitiveDataType.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION ENUM.
///////////////////////////////////////////////////////////////////////////////////////////////////

class ReflectionEnum
{
    SE_MAKE_NONCOPYABLE(ReflectionEnum);
    SE_MAKE_NONMOVABLE(ReflectionEnum);

public:
    struct Member
    {
        String Name;
        uint64 Value;
    };

public:
    ReflectionEnum() = default;
    ~ReflectionEnum() = default;

    NODISCARD FORCEINLINE UUID GetEnumUUID() const { return m_EnumUUID; }
    NODISCARD FORCEINLINE const String& GetName() const { return m_Name; }
    NODISCARD FORCEINLINE PrimitiveDataType GetUnderlyingDataType() const { return m_UnderlyingDataType; }
    NODISCARD FORCEINLINE const Vector<Member>& GetMembers() const { return m_Members; }

    ENGINE_API void SetEnumUUID(UUID enumUUID);
    ENGINE_API void SetName(String name);
    ENGINE_API void SetUnderlyingDataType(PrimitiveDataType underlyingDataType);
    ENGINE_API void AddMember(Member enumMember);

private:
    UUID m_EnumUUID { UUID::Invalid() };
    String m_Name;
    PrimitiveDataType m_UnderlyingDataType { PrimitiveDataType::Unknown };
    Vector<Member> m_Members;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION ENUM METADATA.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct ReflectionEnumMetadata
{
    SE_MAKE_NAMESPACE_CLASS(ReflectionEnumMetadata);

public:
    static constexpr bool IsSpecialized = false;
    static constexpr UUID EnumUUID { UUID::Invalid() };
    using UnderlyingType = void;
};

#define SE_REFLECTION_ENUM_METADATA(EnumName, EnumUnderlyingType, enumUUIDValue)    \
    enum class EnumName : EnumUnderlyingType;                                       \
    template<>                                                                      \
    struct ReflectionEnumMetadata<EnumName>                                         \
    {                                                                               \
        SE_MAKE_NAMESPACE_CLASS(ReflectionEnumMetadata);                            \
    public:                                                                         \
        static constexpr bool IsSpecialized { true };                               \
        static constexpr UUID EnumUUID { enumUUIDValue };                           \
        using UnderlyingType = EnumUnderlyingType;                                  \
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION ENUM MACROS.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SE_BEGIN_ENUM_REFLECTION(EnumName)                                                                                  \
    {                                                                                                                       \
        using Metadata = ReflectionEnumMetadata<EnumName>;                                                                  \
        static_assert(Metadata::IsSpecialized, "Declare 'SE_REFLECTION_ENUM_METADATA(EnumName)' before using this macro!"); \
        ReflectionEnum& reflectionEnum = SceneReflectionRegistry::CreateEnumFromUUID(Metadata::EnumUUID);                   \
        reflectionEnum.SetEnumUUID(Metadata::EnumUUID);                                                                     \
        reflectionEnum.SetName(VIEW(#EnumName));                                                                            \
        static_assert(                                                                                                      \
            std::is_same_v<Metadata::UnderlyingType, std::underlying_type_t<EnumName>>,                                     \
            "The underlying type of the enum doesn't match the one provided in the reflection metadata!");                  \
        reflectionEnum.SetUnderlyingDataType(PrimitiveDataTypeFinder<Metadata::UnderlyingType>::GetDataType());             \
        using EnumType = EnumName;

#define SE_ENUM_MEMBER(MemberName)                                                                                          \
        {                                                                                                                   \
            ReflectionEnum::Member member = {};                                                                             \
            member.Name = VIEW(#MemberName);                                                                                \
            member.Value = static_cast<uint64>(EnumType::MemberName);                                                       \
            reflectionEnum.AddMember(Move(member));                                                                         \
        }

#define SE_END_ENUM_REFLECTION()                                                                                            \
    }

}
