// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/CoreObject/Forward.h>
#include <Runtime/CoreObject/Pointers/WObjectPtr.h>

namespace SE
{

template<typename T>
concept IsStructObject = requires { T::StructTag(); };

///////////////////////////////////////////////////////////////////////////////////////////////////

enum class ObjectTypeKind : uint8
{
#define SE_ENUMERATE_OBJECT_TYPE_KINDS(x)   \
    x(Primitive)                            \
    x(Pointer)                              \
    x(Struct)                               \
    x(Enum)                                 \
    x(String)                               \
    x(Vector)                               \
    x(HashSet)                              \
    x(HashMap)

#define _SE_ENUM_MEMBER(x) x,
    SE_ENUMERATE_OBJECT_TYPE_KINDS(_SE_ENUM_MEMBER)
#undef _SE_ENUM_MEMBER
};

NODISCARD SHOOTER_API String ObjectTypeKindToString(ObjectTypeKind objectTypeKind);
NODISCARD SHOOTER_API StringView ObjectTypeKindToStringView(ObjectTypeKind objectTypeKind);
NODISCARD SHOOTER_API const char* ObjectTypeKindToRawString(ObjectTypeKind objectTypeKind);

NODISCARD SHOOTER_API ObjectTypeKind ObjectTypeKindFromString(StringView string);

class ObjectType
{
    SE_MAKE_NONCOPYABLE(ObjectType);
    SE_MAKE_NONMOVABLE(ObjectType);

public:
    virtual ~ObjectType() = default;
    FORCEINLINE ObjectType(ObjectTypeKind kind)
        : m_Kind(kind)
    {}

    NODISCARD FORCEINLINE ObjectTypeKind GetKind() const { return m_Kind; }

#define _SE_DECLARE_AS_FUNCTION(x) NODISCARD FORCEINLINE const class x##ObjectType& As##x() const { return (const class x##ObjectType&)(*this); }
    SE_ENUMERATE_OBJECT_TYPE_KINDS(_SE_DECLARE_AS_FUNCTION)
#undef _SE_DECLARE_AS_FUNCTION

private:
    ObjectTypeKind m_Kind;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

enum class PrimitiveType
{
#define SE_ENUMERATE_PRIMITIVE_TYPES(x) \
    x(Unknown)                          \
    x(Int8)                             \
    x(Int16)                            \
    x(Int32)                            \
    x(Int64)                            \
    x(UInt8)                            \
    x(UInt16)                           \
    x(UInt32)                           \
    x(UInt64)                           \
    x(Float32)                          \
    x(Float64)                          \
    x(Bool8)                            \
    x(Vector2Float32)                   \
    x(Vector3Float32)                   \
    x(Vector4Float32)

#define _SE_ENUM_MEMBER(x) x,
    SE_ENUMERATE_PRIMITIVE_TYPES(_SE_ENUM_MEMBER)
#undef _SE_ENUM_MEMBER
};

NODISCARD SHOOTER_API String PrimitiveTypeToString(PrimitiveType primitiveType);
NODISCARD SHOOTER_API StringView PrimitiveTypeToStringView(PrimitiveType primitiveType);
NODISCARD SHOOTER_API const char* PrimitiveTypeToRawString(PrimitiveType primitiveType);

NODISCARD SHOOTER_API PrimitiveType PrimitiveTypeFromString(StringView string);

class PrimitiveObjectType : public ObjectType
{
public:
    virtual ~PrimitiveObjectType() override = default;
    FORCEINLINE PrimitiveObjectType(PrimitiveType primitiveType)
        : ObjectType(GetStaticKind())
        , m_PrimitiveType(primitiveType)
    {}
    
    NODISCARD FORCEINLINE static ObjectTypeKind GetStaticKind() { return ObjectTypeKind::Primitive; }
    NODISCARD FORCEINLINE PrimitiveType GetPrimitiveType() const { return m_PrimitiveType; }

private:
    PrimitiveType m_PrimitiveType;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

enum class PointerType : uint8
{
    Strong,
    Weak,
};

class PointerObjectType : public ObjectType
{
public:
    virtual ~PointerObjectType() override = default;
    FORCEINLINE PointerObjectType(WObjectPtr<ObjectClass> pointerClass, PointerType pointerType)
        : ObjectType(GetStaticKind())
        , m_PointerClass(pointerClass)
        , m_PointerType(pointerType)
    {}

    NODISCARD FORCEINLINE static ObjectTypeKind GetStaticKind() { return ObjectTypeKind::Pointer; }
    NODISCARD FORCEINLINE WObjectPtr<ObjectClass> GetPointerClass() const { return m_PointerClass; }
    NODISCARD FORCEINLINE PointerType GetPointerType() const { return m_PointerType; }

private:
    WObjectPtr<ObjectClass> m_PointerClass;
    PointerType m_PointerType;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class StructObjectType : public ObjectType
{
public:
    virtual ~StructObjectType() override = default;
    FORCEINLINE StructObjectType(WObjectPtr<ObjectStruct> objectStruct)
        : ObjectType(GetStaticKind())
        , m_Struct(objectStruct)
    {}

    NODISCARD FORCEINLINE static ObjectTypeKind GetStaticKind() { return ObjectTypeKind::Struct; }
    NODISCARD FORCEINLINE WObjectPtr<ObjectStruct> GetStruct() const { return m_Struct; }

private:
    WObjectPtr<ObjectStruct> m_Struct;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class EnumObjectType : public ObjectType
{
public:
    virtual ~EnumObjectType() override = default;
    FORCEINLINE EnumObjectType(WObjectPtr<ObjectEnum> objectEnum)
        : ObjectType(GetStaticKind())
        , m_Enum(objectEnum)
    {}

    NODISCARD FORCEINLINE static ObjectTypeKind GetStaticKind() { return ObjectTypeKind::Enum; }
    NODISCARD FORCEINLINE WObjectPtr<ObjectEnum> GetEnum() const { return m_Enum; }

private:
    WObjectPtr<ObjectEnum> m_Enum;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class StringObjectType : public ObjectType
{
public:
    virtual ~StringObjectType() override = default;
    FORCEINLINE StringObjectType()
        : ObjectType(GetStaticKind())
    {}

    NODISCARD FORCEINLINE static ObjectTypeKind GetStaticKind() { return ObjectTypeKind::String; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class VectorObjectType : public ObjectType
{
public:
    virtual ~VectorObjectType() override = default;
    FORCEINLINE VectorObjectType(const ObjectType* elementsType)
        : ObjectType(GetStaticKind())
        , m_ElementsType(elementsType)
    {}

    NODISCARD FORCEINLINE static ObjectTypeKind GetStaticKind() { return ObjectTypeKind::Vector; }
    NODISCARD FORCEINLINE const ObjectType* GetElementsType() const { return m_ElementsType; }

private:
    const ObjectType* m_ElementsType;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class HashSetObjectType : public ObjectType
{
public:
    virtual ~HashSetObjectType() override = default;
    FORCEINLINE HashSetObjectType(const ObjectType* elementsType)
        : ObjectType(GetStaticKind())
        , m_ElementsType(elementsType)
    {}

    NODISCARD FORCEINLINE static ObjectTypeKind GetStaticKind() { return ObjectTypeKind::HashSet; }
    NODISCARD FORCEINLINE const ObjectType* GetElementsType() const { return m_ElementsType; }

private:
    const ObjectType* m_ElementsType;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class HashMapObjectType : public ObjectType
{
public:
    virtual ~HashMapObjectType() override = default;
    FORCEINLINE HashMapObjectType(const ObjectType* keysType, const ObjectType* valuesType)
        : ObjectType(GetStaticKind())
        , m_KeysType(keysType)
        , m_ValuesType(valuesType)
    {}

    NODISCARD FORCEINLINE static ObjectTypeKind GetStaticKind() { return ObjectTypeKind::HashMap; }
    NODISCARD FORCEINLINE const ObjectType* GetKeysType() const { return m_KeysType; }
    NODISCARD FORCEINLINE const ObjectType* GetValuesType() const { return m_ValuesType; }

private:
    const ObjectType* m_KeysType;
    const ObjectType* m_ValuesType;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

}
