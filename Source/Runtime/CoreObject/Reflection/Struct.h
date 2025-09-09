// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Containers/HashSet.h>
#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Math/Vector.h>
#include <Runtime/CoreObject/Forward.h>
#include <Runtime/CoreObject/Object.h>
#include <Runtime/CoreObject/Reflection/Type.h>

namespace SE
{

union ObjectFieldValue
{
public:
    FORCEINLINE ObjectFieldValue()
    {
        MemoryZero(ValuePointer, sizeof(ValuePointer));
    }

    FORCEINLINE ObjectFieldValue(const ObjectFieldValue& other)
    {
        MemoryCopy(ValuePointer, other.ValuePointer, sizeof(ValuePointer));
    }

public:
    int8    I8;    int16   I16;   int32   I32; int64  I64;
    uint8   U8;    uint16  U16;   uint32  U32; uint64 U64;
    Float32 F32;   Float64 F64;
    bool    Bool;
    Vector2 V2F32; Vector3 V3F32; Vector4 V4F32;
    uint64  Enum;
    
    // Should be the same size as the largest typed value.
    uint8 ValuePointer[sizeof(Vector4)];
};

struct ObjectField
{
public:
    ObjectField() = default;
    ObjectField(const ObjectField&) = default;
    ObjectField& operator=(const ObjectField&) = default;

public:
    String      Name;
    ObjectType* Type       { nullptr };
    usize       ByteOffset { 0 };
    usize       ByteCount  { 0 };
    uint32      ArrayCount { 0 };
};

class ObjectStruct : public Object
{
public:
    ObjectStruct(const ObjectInitializer& objectInitializer);
    virtual ~ObjectStruct() override = default;
    NODISCARD virtual SObjectPtr<ObjectClass> GetClass() const { return {}; }

public:
    NODISCARD SHOOTER_API bool IsDerivedFrom(SObjectPtr<ObjectStruct> baseObjectStruct) const;
    NODISCARD SHOOTER_API const HashSet<SObjectPtr<ObjectStruct>>& GetParentChain() const;

public:
    NODISCARD FORCEINLINE const String& GetName() const { return m_Name; }
    NODISCARD FORCEINLINE usize GetStructureByteCount() const { return m_StructureByteCount; }

    NODISCARD FORCEINLINE const HashMap<String, ObjectField>& GetNonInheritedFields() const { return m_NonInheritedFields; }
    NODISCARD FORCEINLINE const HashMap<String, ObjectFieldValue>& GetFieldDefaultValues() const { return m_FieldDefaultValues; }
    NODISCARD SHOOTER_API const HashMap<String, ObjectField>& GetFields() const;

    SHOOTER_API void SetName(String name);
    SHOOTER_API void SetStructureByteCount(usize structureByteCount);
    SHOOTER_API void SetParent(SObjectPtr<ObjectStruct> parent);

    SHOOTER_API void AddNonInheritedField(const ObjectField& field);
    SHOOTER_API void AddFieldDefaultValue(String fieldName, const ObjectFieldValue& defaultValue);

private:
    String m_Name;
    usize m_StructureByteCount { 0 };
    SObjectPtr<ObjectStruct> m_Parent;
    HashMap<String, ObjectField> m_NonInheritedFields;
    HashMap<String, ObjectFieldValue> m_FieldDefaultValues;

    // NOTE(Traian): Functions such as 'GetParentChain' and 'GetFields' will lazily create these data
    // structures, as they require recursive calls to the structure parents.
    mutable HashMap<String, ObjectField> m_CachedFields;
    mutable HashSet<SObjectPtr<ObjectStruct>> m_CachedParentChain;
};

}
