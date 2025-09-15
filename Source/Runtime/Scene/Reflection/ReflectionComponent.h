// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Scene/Reflection/ReflectionStruct.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION COMPONENT.
///////////////////////////////////////////////////////////////////////////////////////////////////

using PFN_ConstructComponent = void(*)(void*);

using PFN_DestructComponent = void(*)(void*);

using PFN_CopyComponent = void(*)(void*, const void*);

class ReflectionComponent
{
    SE_MAKE_NONCOPYABLE(ReflectionComponent);
    SE_MAKE_NONMOVABLE(ReflectionComponent);

public:
    ReflectionComponent() = default;
    ~ReflectionComponent() = default;

    NODISCARD FORCEINLINE UUID GetComponentUUID() const { return m_ComponentUUID; }
    NODISCARD FORCEINLINE const String& GetName() const { return m_Name; }

    // NOTE(Traian): The reflection struct returned by these methods are not the same as
    // the ones created directly by the 'SceneReflectionRegistry'. They only hold information
    // regarding the fields of the script and the memory layout, not about UUID/name.
    NODISCARD FORCEINLINE ReflectionStruct& GetStruct() { return m_Struct; }
    NODISCARD FORCEINLINE const ReflectionStruct& GetStruct() const { return m_Struct; }

    ENGINE_API void SetComponentUUID(UUID componentUUID);
    ENGINE_API void SetName(String name);

    ENGINE_API void SetConstructFunction(PFN_ConstructComponent function);
    ENGINE_API void SetDestructFunction(PFN_DestructComponent function);
    ENGINE_API void SetCopyFunction(PFN_CopyComponent function);

    ENGINE_API void ExecuteConstruct(void* dstMemoryBlock);
    ENGINE_API void ExecuteDestruct(void* dstMemoryBlock);
    ENGINE_API void ExecuteCopy(void* dstMemoryBlock, const void* srcMemoryBlock);

private:
    UUID m_ComponentUUID;
    String m_Name;
    ReflectionStruct m_Struct;

    PFN_ConstructComponent m_ConstructFunction;
    PFN_DestructComponent m_DestructFunction;
    PFN_CopyComponent m_CopyFunction;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION COMPONENT METADATA.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct ReflectionComponentMetadata
{
    SE_MAKE_NAMESPACE_CLASS(ReflectionComponentMetadata);

public:
    static constexpr bool IsSpecialized { false };
    static constexpr UUID ComponentUUID { UUID::Invalid() };
};

#define SE_REFLECTION_COMPONENT_METADATA(ComponentName, componentUUIDValue) \
    template<>                                                              \
    struct ReflectionComponentMetadata<struct ComponentName>                \
    {                                                                       \
        SE_MAKE_NAMESPACE_CLASS(ReflectionComponentMetadata);               \
    public:                                                                 \
        static constexpr bool IsSpecialized { true };                       \
        static constexpr UUID ComponentUUID { componentUUIDValue };         \
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION COMPONENT MACROS.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SE_BEGIN_COMPONENT_REFLECTION(ComponentName)                                                                                    \
    {                                                                                                                                   \
        using Metadata = ReflectionComponentMetadata<ComponentName>;                                                                    \
        static_assert(Metadata::IsSpecialized, "Declare 'SE_REFLECTION_COMPONENT_METADATA(ComponentName)' before using this macro!");   \
        ReflectionComponent& reflectionComponent = SceneReflectionRegistry::CreateComponentFromUUID(Metadata::ComponentUUID);           \
        reflectionComponent.SetComponentUUID(Metadata::ComponentUUID);                                                                  \
        reflectionComponent.SetName(VIEW(#ComponentName));                                                                              \
        reflectionComponent.SetConstructFunction(                                                                                       \
            [](void* dstMemoryBlock)                                                                                                    \
            {                                                                                                                           \
                new (dstMemoryBlock) ComponentName();                                                                                   \
            });                                                                                                                         \
        reflectionComponent.SetDestructFunction(                                                                                        \
            [](void* dstMemoryBlock)                                                                                                    \
            {                                                                                                                           \
                static_cast<ComponentName*>(dstMemoryBlock)->~ComponentName();                                                          \
            });                                                                                                                         \
        reflectionComponent.SetCopyFunction(                                                                                            \
            [](void* dstMemoryBlock, const void* srcMemoryBlock)                                                                        \
            {                                                                                                                           \
                const ComponentName* src = static_cast<const ComponentName*>(srcMemoryBlock);                                           \
                new (dstMemoryBlock) ComponentName(*src);                                                                               \
            });                                                                                                                         \
        ReflectionStruct& reflectionStruct = reflectionComponent.GetStruct();                                                           \
        reflectionStruct.SetStructureByteCount(sizeof(ComponentName));                                                                  \
        using StructType = ComponentName;

#define SE_COMPONENT_FIELD(FieldName)                                                                                                   \
        SE_STRUCT_FIELD(FieldName)

#define SE_END_COMPONENT_REFLECTION()                                                                                                   \
    }

}
