// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Scene/Reflection/Fields/FieldType.h>
#include <Runtime/Scene/Reflection/PrimitiveDataType.h>
#include <Runtime/Scene/Reflection/ReflectionEnum.h>
#include <Runtime/Scene/Reflection/ReflectionStruct.h>
#include <Runtime/Scene/Reflection/SceneReflectionRegistry.h>

namespace SE
{

template<typename T>
struct FieldTypeFinder
{
    NODISCARD FORCEINLINE static constexpr usize GetRequiredMemory()
    {
        static_assert(false, "You must specialize FieldTypeFinder<T> in order to use this type!");
        return 0;
    }

    NODISCARD FORCEINLINE static FieldType* AllocateFieldType(void* memoryBlock)
    {
        static_assert(false, "You must specialize FieldTypeFinder<T> in order to use this type!");
        return nullptr;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////// FIELD PRIMITIVE TYPE FINDER.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _SE_DECLARE_FIELD_PRIMITIVE_TYPE_FINDER(Enum, Type)                             \
    template<>                                                                          \
    struct FieldTypeFinder<Type>                                                        \
    {                                                                                   \
        NODISCARD FORCEINLINE static constexpr usize GetRequiredMemory()                \
        {                                                                               \
            return sizeof(FieldPrimitiveType);                                          \
        }                                                                               \
                                                                                        \
        NODISCARD FORCEINLINE static FieldType* AllocateFieldType(void* memoryBlock)    \
        {                                                                               \
            return new (memoryBlock) FieldPrimitiveType(PrimitiveDataType::Enum);       \
        }                                                                               \
    };
SE_ENUMERATE_PRIMITIVE_DATA_TYPES(_SE_DECLARE_FIELD_PRIMITIVE_TYPE_FINDER)
#undef _SE_DECLARE_FIELD_PRIMITIVE_TYPE_FINDER

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////// FIELD ENUM TYPE FINDER.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
requires (std::is_enum_v<T>)
struct FieldTypeFinder<T>
{
    NODISCARD FORCEINLINE static constexpr usize GetRequiredMemory()
    {
        return sizeof(FieldEnumType);
    }

    NODISCARD FORCEINLINE static FieldType* AllocateFieldType(void* memoryBlock)
    {
        constexpr UUID enumUUID = ReflectionEnumMetadata<T>::EnumUUID;
        return new (memoryBlock) FieldEnumType(enumUUID);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////// FIELD STRUCT TYPE FINDER.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
requires (ReflectionStructUUID<T>::IsStructType)
struct FieldTypeFinder<T>
{
    NODISCARD FORCEINLINE static constexpr usize GetRequiredMemory()
    {
        return sizeof(FieldStructType);
    }

    NODISCARD FORCEINLINE static FieldType* AllocateFieldType(void* memoryBlock)
    {
        constexpr UUID structUUID = ReflectionStructMetadata<T>::StructUUID;
        return new (memoryBlock) FieldStructType(structUUID);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////// FIELD STRING TYPE FINDER.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<>
struct FieldTypeFinder<String>
{
    NODISCARD FORCEINLINE static constexpr usize GetRequiredMemory()
    {
        return sizeof(FieldStringType);
    }

    NODISCARD FORCEINLINE static FieldType* AllocateFieldType(void* memoryBlock)
    {
        return new (memoryBlock) FieldStringType();
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////// FIELD VECTOR TYPE FINDER.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ElementType, typename Allocator>
struct FieldTypeFinder<Vector<ElementType, Allocator>>
{
    NODISCARD FORCEINLINE static constexpr usize GetRequiredMemory()
    {
        constexpr usize elementTypeRequiredMemory = FieldTypeFinder<ElementType>::GetRequiredMemory();
        return elementTypeRequiredMemory + sizeof(FieldTypedVectorType<ElementType, Allocator>);
    }

    NODISCARD FORCEINLINE static FieldType* AllocateFieldType(void* memoryBlock)
    {
        constexpr usize vectorRequiredMemory = sizeof(FieldTypedVectorType<ElementType, Allocator>);
        constexpr usize elementTypeRequiredMemory = FieldTypeFinder<ElementType>::GetRequiredMemory();

        // NOTE(Traian): Allocate the required memory blocks for each field type in the same order they
        // will be accessed later on (to optimize cache lines).
        constexpr usize vectorByteOffset = 0;
        constexpr usize elementTypeByteOffset = vectorRequiredMemory;
        uint8* bytes = static_cast<uint8*>(memoryBlock);

        FieldType* elementType = FieldTypeFinder<ElementType>::AllocateFieldType(bytes + elementTypeByteOffset);
        return new (bytes + vectorByteOffset) FieldTypedVectorType<ElementType, Allocator>(elementType);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////// FIELD HASH SET TYPE FINDER.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ElementType, typename Allocator>
struct FieldTypeFinder<HashSet<ElementType, Allocator>>
{
    NODISCARD FORCEINLINE static constexpr usize GetRequiredMemory()
    {
        constexpr usize elementTypeRequiredMemory = FieldTypeFinder<ElementType>::GetRequiredMemory();
        return elementTypeRequiredMemory + sizeof(FieldTypedHashSetType<ElementType, Allocator>);
    }

    NODISCARD FORCEINLINE static FieldType* AllocateFieldType(void* memoryBlock)
    {
        constexpr usize hashSetRequiredMemory = sizeof(FieldTypedHashSetType<ElementType, Allocator>);
        constexpr usize elementTypeRequiredMemory = FieldTypeFinder<ElementType>::GetRequiredMemory();

        // NOTE(Traian): Allocate the required memory blocks for each field type in the same order they
        // will be accessed later on (to optimize cache lines).
        constexpr usize hashSetByteOffset = 0;
        constexpr usize elementTypeByteOffset = hashSetRequiredMemory;
        uint8* bytes = static_cast<uint8*>(memoryBlock);

        FieldType* elementType = FieldTypeFinder<ElementType>::AllocateFieldType(bytes + elementTypeByteOffset);
        return new (bytes + hashSetByteOffset) FieldTypedHashSetType<ElementType, Allocator>(elementType);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////// FIELD HASH MAP TYPE FINDER.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename KeyType, typename ValueType, typename Allocator>
struct FieldTypeFinder<HashMap<KeyType, ValueType, Allocator>>
{
    NODISCARD FORCEINLINE static constexpr usize GetRequiredMemory()
    {
        constexpr usize keyTypeRequiredMemory = FieldTypeFinder<KeyType>::GetRequiredMemory();
        constexpr usize valueTypeRequiredMemory = FieldTypeFinder<ValueType>::GetRequiredMemory();
        return (keyTypeRequiredMemory + valueTypeRequiredMemory) + sizeof(FieldTypedHashMapType<KeyType, ValueType, Allocator>);
    }

    NODISCARD FORCEINLINE static FieldType* AllocateFieldType(void* memoryBlock)
    {
        constexpr usize hashMapRequiredMemory = sizeof(FieldTypedHashMapType<KeyType, ValueType, Allocator>);
        constexpr usize keyTypeRequiredMemory = FieldTypeFinder<KeyType>::GetRequiredMemory();
        constexpr usize valueTypeRequiredMemory = FieldTypeFinder<ValueType>::GetRequiredMemory();

        // NOTE(Traian): Allocate the required memory blocks for each field type in the same order they
        // will be accessed later on (to optimize cache lines).
        constexpr usize hashMapByteOffset = 0;
        constexpr usize keyTypeByteOffset = hashMapRequiredMemory;
        constexpr usize valueTypeByteOffset = hashMapRequiredMemory + keyTypeRequiredMemory;
        uint8* bytes = static_cast<uint8*>(memoryBlock);

        FieldType* keyType = FieldTypeFinder<KeyType>::AllocateFieldType(bytes + keyTypeByteOffset);
        FieldType* valueType = FieldTypeFinder<ValueType>::AllocateFieldType(bytes + valueTypeByteOffset);

        return new (bytes + hashMapByteOffset) FieldTypedHashMapType<KeyType, ValueType, Allocator>(keyType, valueType);
    }
};

}
