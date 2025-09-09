// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Math/Vector.h>
#include <Runtime/CoreObject/Forward.h>
#include <Runtime/CoreObject/Object.h>
#include <Runtime/CoreObject/Reflection/Type.h>

namespace SE
{

struct ObjectField
{
public:
    ObjectField() = default;

    FORCEINLINE ObjectField(const ObjectField& other)
        : Name(other.Name)
        , Type(other.Type)
        , ByteOffset(other.ByteOffset)
        , ByteCount(other.ByteCount)
        , ArrayCount(other.ArrayCount)
    {
        // Copy the default value buffer.
        MemoryCopy(DefaultValue.ValuePointer, other.DefaultValue.ValuePointer, sizeof(DefaultValue.ValuePointer));
    }

public:
    String      Name;
    ObjectType* Type       { nullptr };
    usize       ByteOffset { 0 };
    usize       ByteCount  { 0 };
    uint32      ArrayCount { 0 };

public:
    union
    {
        // NOTE(Traian): Should be equal in size with the largest primitive type.
        uint8 ValuePointer[sizeof(Vector4)] {};

        uint64 Enum;
        int8 I8;
        int16 I16;
        int32 I32;
        int64 I64;
        uint8 U8;
        uint16 U16;
        uint32 U32;
        uint64 U64;
        Float32 F32;
        Float64 F64;
        Vector2 V2F32;
        Vector3 V3F32;
        Vector4 V4F32;
    }
    DefaultValue;
};

class ObjectStruct : public Object
{
public:
    ObjectStruct(const ObjectInitializer& objectInitializer);
    virtual ~ObjectStruct() override = default;
    NODISCARD virtual SObjectPtr<ObjectClass> GetClass() const { return {}; }

public:
    NODISCARD FORCEINLINE const String& GetName() const { return m_Name; }
    NODISCARD FORCEINLINE usize GetStructureByteCount() const { return m_StructureByteCount; }
    NODISCARD FORCEINLINE SObjectPtr<ObjectStruct> GetParent() const { return m_Parent; }
    NODISCARD FORCEINLINE const Vector<ObjectField>& GetFields() const { return m_Fields; }

    SHOOTER_API void SetName(String name);
    SHOOTER_API void SetStructureByteCount(usize structureByteCount);
    SHOOTER_API void SetParent(SObjectPtr<ObjectStruct> parent);
    SHOOTER_API void AddField(const ObjectField& field);

    NODISCARD SHOOTER_API Optional<ObjectField> GetFieldFromName(StringView name) const;

private:
    String m_Name;
    usize m_StructureByteCount { 0 };
    SObjectPtr<ObjectStruct> m_Parent;
    Vector<ObjectField> m_Fields;
};

}
