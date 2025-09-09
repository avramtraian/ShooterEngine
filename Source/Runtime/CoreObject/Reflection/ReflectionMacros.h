// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Containers/HashSet.h>
#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Math/Vector.h>
#include <Runtime/CoreObject/Forward.h>
#include <Runtime/CoreObject/GlobalEnvironment.h>
#include <Runtime/CoreObject/Reflection/Class.h>
#include <Runtime/CoreObject/Reflection/Enum.h>
#include <Runtime/CoreObject/Reflection/Struct.h>
#include <Runtime/CoreObject/Reflection/Type.h>

#include <concepts>
#include <type_traits>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// ENUM NAME FINDER. ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct EnumNameFinder
{
    NODISCARD FORCEINLINE static StringView Get()
    {
        static_assert(false, "EnumNameFinder<T> can only be used when T is an enum!");
        return {};
    }
};

template<typename T>
requires (std::is_enum_v<T>)
struct EnumNameFinder<T>
{
    NODISCARD FORCEINLINE static StringView Get()
    {
        static_assert(false, "You must use 'SE_DECLARE_ENUM_NAME_FINDER(T)' before using EnumNameFinder<T>!");
        return {};
    }
};

#define SE_DECLARE_ENUM_NAME_FINDER(EnumName)                                                                           \
    template<>                                                                                                          \
    struct EnumNameFinder<EnumName>                                                                                     \
    {                                                                                                                   \
        NODISCARD FORCEINLINE static StringView Get()                                                                   \
        {                                                                                                               \
            static_assert(std::is_enum_v<EnumName>, "When using 'SE_DECLARE_ENUM_NAME_FINDER(T)', T must be an enum!"); \
            return VIEW(#EnumName);                                                                                     \
        }                                                                                                               \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// PRIMITIVE TYPE FINDER. //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct PrimitiveTypeFinder
{
    NODISCARD FORCEINLINE static constexpr PrimitiveType Get()
    {
        static_assert(false, "You must specialize PrimitiveTypeFinder<T>!");
        return PrimitiveType::Unknown;
    }

    NODISCARD FORCEINLINE static constexpr bool IsPrimitiveType()
    {
        return false;
    }
};

#define SE_DECLARE_PRIMITIVE_TYPE_FINDER(DataType, Type)                \
    template<>                                                          \
    struct PrimitiveTypeFinder<DataType>                                \
    {                                                                   \
        NODISCARD FORCEINLINE static constexpr PrimitiveType Get()      \
        {                                                               \
            return PrimitiveType::Type;                                 \
        }                                                               \
                                                                        \
        NODISCARD FORCEINLINE static constexpr bool IsPrimitiveType()   \
        {                                                               \
            return true;                                                \
        }                                                               \
    };

SE_DECLARE_PRIMITIVE_TYPE_FINDER(int8, Int8);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(int16, Int16);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(int32, Int32);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(int64, Int64);

SE_DECLARE_PRIMITIVE_TYPE_FINDER(uint8, UInt8);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(uint16, UInt16);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(uint32, UInt32);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(uint64, UInt64);

SE_DECLARE_PRIMITIVE_TYPE_FINDER(Float32, Float32);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(Float64, Float64);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(Bool8, Bool8);

SE_DECLARE_PRIMITIVE_TYPE_FINDER(Vector2, Vector2Float32);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(Vector3, Vector3Float32);
SE_DECLARE_PRIMITIVE_TYPE_FINDER(Vector4, Vector4Float32);

#undef SE_DECLARE_PRIMITIVE_OBJECT_TYPE_FINDER

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// OBJECT TYPE FINDER. ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct ObjectTypeFinder
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        static_assert(false, "You must specialize ObjectTypeFinder<T>!");
        return nullptr;
    }
};

template<typename T>
requires (PrimitiveTypeFinder<T>::IsPrimitiveType())
struct ObjectTypeFinder<T>
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        constexpr PrimitiveType primitiveType = PrimitiveTypeFinder<T>::Get();
        return GlobalObjectEnvironment::AllocateObjectType<PrimitiveObjectType>(primitiveType);
    }
};

template<IsStructObject T>
struct ObjectTypeFinder<T>
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        SObjectPtr<ObjectStruct> objectStruct = GlobalObjectEnvironment::FindOrCreateObjectStructByName(T::StaticName);
        return GlobalObjectEnvironment::AllocateObjectType<StructObjectType>(objectStruct);
    }
};

template<typename T>
requires (std::is_enum_v<T>)
struct ObjectTypeFinder<T>
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        SObjectPtr<ObjectEnum> objectEnum = GlobalObjectEnvironment::FindOrCreateObjectEnumByName(EnumNameFinder<T>::Get());
        return GlobalObjectEnvironment::AllocateObjectType<EnumObjectType>(objectEnum);
    }
};

template<typename T>
struct ObjectTypeFinder<SObjectPtr<T>>
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        static_assert(std::is_base_of_v<Object, T>, "When using ObjectTypeFinder<SObjectPtr<T>>, T must derived from 'Object'!");
        SObjectPtr<ObjectClass> pointerClass = T::GetStaticClass();
        return GlobalObjectEnvironment::AllocateObjectType<PointerObjectType>(pointerClass, PointerType::Strong);
    }
};

template<typename T>
struct ObjectTypeFinder<WObjectPtr<T>>
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        static_assert(std::is_base_of_v<Object, T>, "When using ObjectTypeFinder<WObjectPtr<T>>, T must derived from 'Object'!");
        SObjectPtr<ObjectClass> pointerClass = T::GetStaticClass();
        return GlobalObjectEnvironment::AllocateObjectType<PointerObjectType>(pointerClass, PointerType::Weak);
    }
};

template<>
struct ObjectTypeFinder<String>
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        return GlobalObjectEnvironment::AllocateObjectType<StringObjectType>();
    }
};

template<typename ElementType, typename Allocator>
struct ObjectTypeFinder<Vector<ElementType, Allocator>>
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        const ObjectType* elementsType = ObjectTypeFinder<ElementType>::Get();
        return GlobalObjectEnvironment::AllocateObjectType<VectorObjectType>(elementsType);
    }
};

template<typename ElementType, typename Allocator>
struct ObjectTypeFinder<HashSet<ElementType, Allocator>>
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        const ObjectType* elementsType = ObjectTypeFinder<ElementType>::Get();
        return GlobalObjectEnvironment::AllocateObjectType<HashSetObjectType>(elementsType);
    }
};

template<typename KeyType, typename ValueType, typename Allocator>
struct ObjectTypeFinder<HashMap<KeyType, ValueType, Allocator>>
{
    NODISCARD FORCEINLINE static ObjectType* Get()
    {
        const ObjectType* keysType = ObjectTypeFinder<KeyType>::Get();
        const ObjectType* valuesType = ObjectTypeFinder<ValueType>::Get();
        return GlobalObjectEnvironment::AllocateObjectType<HashMapObjectType>(keysType, valuesType);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// ENUM REFLECTION MACROS. /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SE_BEGIN_ENUM_REFLECTION(EnumName)                                                                          \
    {                                                                                                               \
        SObjectPtr<ObjectEnum> objectEnum = GlobalObjectEnvironment::FindOrCreateObjectEnumByName(VIEW(#EnumName)); \
        objectEnum->SetEnumName(VIEW(#EnumName));                                                                   \
        objectEnum->SetUnderlyingPrimitiveType(PrimitiveTypeFinder<std::underlying_type_t<EnumName>>::Get());       \
        using EnumType = EnumName;

#define SE_ENUM_MEMBER(MemberName)                                                                                  \
        {                                                                                                           \
            objectEnum->AddEnumMember((uint64)EnumType::MemberName, VIEW(#MemberName));                             \
        }

#define SE_END_ENUM_REFLECTION()                                                                                    \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// STRUCT REFLECTION MACROS. ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SE_BEGIN_STRUCT_REFLECTION(StructName)                                                                              \
    {                                                                                                                       \
        SObjectPtr<ObjectStruct> objectStruct = GlobalObjectEnvironment::FindOrCreateObjectStructByName(VIEW(#StructName)); \
        objectStruct->SetName(VIEW(#StructName));                                                                           \
        objectStruct->SetStructureByteCount(sizeof(StructName));                                                            \
        if constexpr (StructName::HasParent)                                                                                \
        {                                                                                                                   \
            objectStruct->SetParent(GlobalObjectEnvironment::FindOrCreateObjectStructByName(StructName::ParentName));       \
        }                                                                                                                   \
        using StructType = StructName;                                                                                      \
        StructName defaultStructInstance = {};

#define SE_STRUCT_FIELD(FieldName)                                                                                          \
        {                                                                                                                   \
            ObjectField field = {};                                                                                         \
            field.Name = VIEW(#FieldName);                                                                                  \
            field.Type = ObjectTypeFinder<decltype(StructType::FieldName)>::Get();                                          \
            field.ByteOffset = offsetof(StructType, FieldName);                                                             \
            field.ByteCount = sizeof(StructType::FieldName);                                                                \
            field.ArrayCount = 1;                                                                                           \
            objectStruct->AddField(field);                                                                                  \
            if (field.Type->GetKind() == ObjectTypeKind::Primitive || field.Type->GetKind() == ObjectTypeKind::Enum)        \
            {                                                                                                               \
                ObjectFieldValue fieldDefaultValue = {};                                                                    \
                MemoryCopy(fieldDefaultValue.ValuePointer, &defaultStructInstance.FieldName, field.ByteCount);              \
                objectStruct->AddFieldDefaultValue(field.Name, fieldDefaultValue);                                          \
            }                                                                                                               \
        }

#define SE_END_STRUCT_REFLECTION()                                                                                          \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// CLASS REFLECTION MACROS. ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SE_BEGIN_CLASS_REFLECTION(ClassName)                                                                \
    {                                                                                                       \
        SObjectPtr<ObjectClass> objectClass = ClassName::GetStaticClass();                                  \
        objectClass->SetName(VIEW(#ClassName));                                                             \
        objectClass->SetStructureByteCount(sizeof(ClassName));                                              \
        objectClass->SetParent(ClassName::Super::GetStaticClass());                                         \
        objectClass->GetConstructInPlaceDelegate().BindRaw(                                                 \
            [](void* memoryBlock, const ObjectInitializer& objectInitializer) -> Object*                    \
            {                                                                                               \
                return new (memoryBlock) ClassName(objectInitializer);                                      \
            });                                                                                             \
        using ClassType = ClassName;                                                                        \
        SObjectPtr<ClassName> defaultClassInstance = GlobalObjectEnvironment::CreateObject<ClassName>();    \
        SObjectPtr<ObjectStruct> objectStruct = objectClass;                                                \
        using StructType = ClassName;                                                                       \
        auto& defaultStructInstance = *defaultClassInstance.Get();

#define SE_CLASS_FIELD(FieldName)                                                                           \
        SE_STRUCT_FIELD(FieldName)

#define SE_END_CLASS_REFLECTION()                                                                           \
    }

}
