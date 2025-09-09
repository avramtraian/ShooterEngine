// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/CoreObject/Forward.h>
#include <Runtime/CoreObject/Object.h>
#include <Runtime/CoreObject/Reflection/Type.h>

namespace SE
{

struct ObjectEnumMember
{
    uint64 Value;
    String Name;
    Optional<String> Description;
};

class ObjectEnum : public Object
{
public:
    ObjectEnum(const ObjectInitializer& objectInitializer);
    virtual ~ObjectEnum() override = default;
    NODISCARD virtual SObjectPtr<ObjectClass> GetClass() const { return {}; }

public:
    NODISCARD FORCEINLINE const String& GetEnumName() const { return m_EnumName; }
    NODISCARD FORCEINLINE PrimitiveType GetUnderlyingPrimitiveType() const { return m_UnderlyingPrimitiveType; }
    NODISCARD FORCEINLINE const Vector<ObjectEnumMember>& GetEnumMembers() const { return m_EnumMembers; }

    SHOOTER_API void SetEnumName(String name);
    SHOOTER_API void SetUnderlyingPrimitiveType(PrimitiveType primitiveType);
    SHOOTER_API void AddEnumMember(uint64 value, String name, Optional<String> description = {});

    NODISCARD SHOOTER_API Optional<ObjectEnumMember> GetEnumMemberFromValue(uint64 value) const;
    NODISCARD SHOOTER_API Optional<ObjectEnumMember> GetEnumMemberFromName(StringView name) const;

private:
    String m_EnumName;
    PrimitiveType m_UnderlyingPrimitiveType { PrimitiveType::Unknown };
    Vector<ObjectEnumMember> m_EnumMembers;
};

}
