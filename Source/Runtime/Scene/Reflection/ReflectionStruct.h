// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Delegate.h>
#include <Runtime/Core/UUID.h>
#include <Runtime/Scene/Reflection/Fields/Field.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION STRUCT.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Constructs a new struct at the provided memory address. It uses the default constructor and
// overrides any data currently stored there.
using PFN_ConstructStruct = void(*)(void*);

// Destructs a struct stored at the given memory address.
using PFN_DestructStruct = void(*)(void*);

// Constructs a new struct at the provided memory address by using the copy constructor (and reading
// the source struct from the second memory address). It overrides any data currently stored there.
using PFN_CopyStruct = void(*)(void*, const void*);

class ReflectionStruct
{
    SE_MAKE_NONCOPYABLE(ReflectionStruct);
    SE_MAKE_NONMOVABLE(ReflectionStruct);

public:
    ReflectionStruct() = default;
    ~ReflectionStruct() = default;

    NODISCARD FORCEINLINE UUID GetStructUUID() const { return m_StructUUID; }
    NODISCARD FORCEINLINE UUID GetParentStructUUID() const { return m_ParentStructUUID; }
    NODISCARD FORCEINLINE const String& GetName() const { return m_Name; }
    NODISCARD FORCEINLINE usize GetStructureByteCount() const { return m_StructureByteCount; }
    NODISCARD FORCEINLINE const Vector<Field>& GetNonInheritedFields() const { return m_NonInheritedFields; }

    RUNTIME_API void SetStructUUID(UUID structUUID);
    RUNTIME_API void SetParentStructUUID(UUID parentStructUUID);
    RUNTIME_API void SetName(String name);
    RUNTIME_API void SetStructureByteCount(usize structureByteCount);

    RUNTIME_API void AddNonInheritedField(Field field);
    
    RUNTIME_API void SetConstructFunction(PFN_ConstructStruct function);
    RUNTIME_API void SetDestructFunction(PFN_DestructStruct function);
    RUNTIME_API void SetCopyFunction(PFN_CopyStruct function);

    RUNTIME_API void ExecuteConstruct(void* dstMemoryBlock);
    RUNTIME_API void ExecuteDestruct(void* dstMemoryBlock);
    RUNTIME_API void ExecuteCopy(void* dstMemoryBlock, const void* srcMemoryBlock);

    NODISCARD RUNTIME_API void* AllocateFieldType(usize fieldTypeByteCount);

private:
    UUID m_StructUUID { UUID::Invalid() };
    UUID m_ParentStructUUID { UUID::Invalid() };
    String m_Name;
    usize m_StructureByteCount { 0 };
    Vector<Field> m_NonInheritedFields;

    PFN_ConstructStruct m_ConstructFunction { nullptr };
    PFN_DestructStruct m_DestructFunction { nullptr };
    PFN_CopyStruct m_CopyFunction { nullptr };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION STRUCT METADATA.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct ReflectionStructMetadata
{
    SE_MAKE_NAMESPACE_CLASS(ReflectionStructMetadata)

public:
    static constexpr bool IsSpecialized = false;
    static constexpr UUID StructUUID { UUID::Invalid() };
    using ParentType = void;
};

template<>
struct ReflectionStructMetadata<void>
{
    SE_MAKE_NAMESPACE_CLASS(ReflectionStructMetadata);

public:
    static constexpr bool IsSpecialized = true;
    static constexpr UUID StructUUID { UUID::Invalid() };
    using ParentType = void;
};

#define SE_REFLECTION_STRUCT_METADATA(StructName, ParentStructName, structUUIDValue)    \
    template<>                                                                          \
    struct ReflectionStructMetadata<struct StructName>                                  \
    {                                                                                   \
        SE_MAKE_NAMESPACE_CLASS(ReflectionStructMetadata);                              \
    public:                                                                             \
        static constexpr bool IsSpecialized = true;                                     \
        static constexpr UUID StructUUID { structUUIDValue };                           \
        using ParentType = ParentStructName;                                            \
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION STRUCT MACROS.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SE_BEGIN_STRUCT_REFLECTION(StructName)                                                                                  \
    {                                                                                                                           \
        using Metadata = ReflectionStructMetadata<StructName>;                                                                  \
        static_assert(Metadata::IsSpecialized, "Declare 'SE_REFLECTION_STRUCT_METADATA(StructName)' before using this macro!"); \
        ReflectionStruct& reflectionStruct = SceneReflectionRegistry::CreateStructFromUUID(Metadata::StructUUID);               \
        reflectionStruct.SetStructUUID(Metadata::StructUUID);                                                                   \
        if constexpr (!std::is_same_v<Metadata::ParentType, void>)                                                              \
        {                                                                                                                       \
            constexpr UUID parentStructUUID = ReflectionStructMetadata<Metadata::ParentType>::StructUUID;                       \
            SE_ASSERT(parentStructUUID != UUID::Invalid());                                                                     \
            reflectionStruct.SetParentStructUUID(parentStructUUID);                                                             \
        }                                                                                                                       \
        reflectionStruct.SetName(VIEW(#StructName));                                                                            \
        reflectionStruct.SetStructureByteCount(sizeof(StructName));                                                             \
        reflectionStruct.SetConstructFunction(                                                                                  \
            [](void* dstMemoryBlock)                                                                                            \
            {                                                                                                                   \
                new (dstMemoryBlock) StructName();                                                                              \
            });                                                                                                                 \
        reflectionStruct.SetDestructFunction(                                                                                   \
            [](void* dstMemoryBlock)                                                                                            \
            {                                                                                                                   \
                static_cast<StructName*>(dstMemoryBlock)->~StructName();                                                        \
            });                                                                                                                 \
        reflectionStruct.SetCopyFunction(                                                                                       \
            [](void* dstMemoryBlock, const void* srcMemoryBlock)                                                                \
            {                                                                                                                   \
                const StructName* srcStruct = static_cast<const StructName*>(srcMemoryBlock);                                   \
                new (dstMemoryBlock) StructName(*srcStruct);                                                                    \
            });                                                                                                                 \
        using StructType = StructName;

#define SE_STRUCT_FIELD(FieldName)                                                                                              \
        {                                                                                                                       \
            const usize fieldTypeRequiredMemory = FieldTypeFinder<decltype(StructType::FieldName)>::GetRequiredMemory();        \
            void* fieldTypeMemoryBlock = reflectionStruct.AllocateFieldType(fieldTypeRequiredMemory);                           \
            FieldType* fieldType = FieldTypeFinder<decltype(StructType::FieldName)>::AllocateFieldType(fieldTypeMemoryBlock);   \
            Field field = {};                                                                                                   \
            field.Type = fieldType;                                                                                             \
            field.Name = VIEW(#FieldName);                                                                                      \
            field.ByteOffset = offsetof(StructType, FieldName);                                                                 \
            field.ByteCount = sizeof(StructType::FieldName);                                                                    \
            reflectionStruct.AddNonInheritedField(Move(field));                                                                 \
        }

#define SE_END_STRUCT_REFLECTION()                                                                                              \
    }

}
