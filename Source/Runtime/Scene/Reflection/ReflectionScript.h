// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Scene/Forward.h>
#include <Runtime/Scene/Reflection/ReflectionStruct.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION SCRIPT.
///////////////////////////////////////////////////////////////////////////////////////////////////

using PFN_ConstructScript = Script* (*)(void*);

class ReflectionScript
{
    SE_MAKE_NONCOPYABLE(ReflectionScript);
    SE_MAKE_NONMOVABLE(ReflectionScript);

public:
    ReflectionScript()  = default;
    ~ReflectionScript() = default;

    NODISCARD ALWAYS_INLINE UUID GetScriptUUID() const { return m_ScriptUUID; }
    NODISCARD ALWAYS_INLINE UUID GetParentScriptUUID() const { return m_ParentScriptUUID; }
    NODISCARD ALWAYS_INLINE const String& GetName() const { return m_Name; }

    // NOTE(Traian): The reflection struct returned by these methods are not the same as
    // the ones created directly by the 'SceneReflectionRegistry'. They only hold information
    // regarding the fields of the script and the memory layout, not about UUID/name/parenting.
    NODISCARD ALWAYS_INLINE ReflectionStruct& GetStruct() { return m_Struct; }
    NODISCARD ALWAYS_INLINE const ReflectionStruct& GetStruct() const { return m_Struct; }

    // NOTE: Returns the list of the script hierarchiy reflection UUIDs, including this script UUID itself.
    NODISCARD RUNTIME_API const HashSet<UUID>& GetParentHierarchiyUUIDs() const;

    RUNTIME_API void SetScriptUUID(UUID scriptUUID);
    RUNTIME_API void SetParentScriptUUID(UUID parentScriptUUID);
    RUNTIME_API void SetName(String name);

    RUNTIME_API void SetConstructFunction(PFN_ConstructScript function);
    RUNTIME_API Script* ExecuteConstruct(void* dstMemoryBlock) const;

private:
    // NOTE: Invoked on-demand when the 'GetParentHierarchiyUUIDs' function is called, the hierarchiy UUID list is empty,
    //       but the script has a parent (which should always be true, since all classes are derived from the base 'Script').
    //       Because it is invoked from 'GetParentHierarchiyUUIDs' it must be marked as const, and that is also the reason
    //       the 'm_ParentHierarchiyUUIDs' field is marked as mutable.
    void RecursiveFillHierarchiyUUIDs() const;

private:
    UUID                  m_ScriptUUID { UUID::Invalid() };
    UUID                  m_ParentScriptUUID { UUID::Invalid() };
    mutable HashSet<UUID> m_ParentHierarchiyUUIDs;
    String                m_Name;
    ReflectionStruct      m_Struct;
    PFN_ConstructScript   m_ConstructFunction { nullptr };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION SCRIPT METADATA.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct ReflectionScriptMetadata
{
    SE_MAKE_NAMESPACE_CLASS(ReflectionScriptMetadata);

public:
    static constexpr bool IsSpecialized { false };
    static constexpr UUID ScriptUUID { UUID::Invalid() };
    using ParentType = void;
};

template<>
struct ReflectionScriptMetadata<void>
{
    SE_MAKE_NAMESPACE_CLASS(ReflectionScriptMetadata);

public:
    static constexpr bool IsSpecialized { true };
    static constexpr UUID ScriptUUID { UUID::Invalid() };
    using ParentType = void;
};

#define SE_REFLECTION_SCRIPT_METADATA(ScriptName, ParentScriptName, scriptUUIDValue) \
    template<>                                                                       \
    struct ReflectionScriptMetadata<class ScriptName>                                \
    {                                                                                \
        SE_MAKE_NAMESPACE_CLASS(ReflectionScriptMetadata);                           \
                                                                                     \
    public:                                                                          \
        static constexpr bool IsSpecialized { true };                                \
        static constexpr UUID ScriptUUID { scriptUUIDValue };                        \
        using ParentType = ParentScriptName;                                         \
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION SCRIPT MACROS.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SE_BEGIN_SCRIPT_REFLECTION(ScriptName)                                                                                    \
    {                                                                                                                             \
        using Metadata = ReflectionScriptMetadata<ScriptName>;                                                                    \
        static_assert(Metadata::IsSpecialized, "Declare 'SE_REFLECTION_SCRIPT_METADATA(ScriptName)' before using this macro!");   \
        ReflectionScript& reflectionScript = SceneReflectionRegistry::CreateScriptFromUUID(Metadata::ScriptUUID);                 \
        reflectionScript.SetScriptUUID(Metadata::ScriptUUID);                                                                     \
        if constexpr (!std::is_same_v<Metadata::ParentType, void>)                                                                \
        {                                                                                                                         \
            constexpr UUID parentScriptUUID = ReflectionScriptMetadata<Metadata::ParentType>::ScriptUUID;                         \
            SE_ASSERT(parentScriptUUID != UUID::Invalid());                                                                       \
            reflectionScript.SetParentScriptUUID(parentScriptUUID);                                                               \
        }                                                                                                                         \
        reflectionScript.SetName(VIEW(#ScriptName));                                                                              \
        reflectionScript.SetConstructFunction([](void* dstMemoryBlock) -> Script* { return new (dstMemoryBlock) ScriptName(); }); \
        ReflectionStruct& reflectionStruct = reflectionScript.GetStruct();                                                        \
        reflectionStruct.SetStructureByteCount(sizeof(ScriptName));                                                               \
        using StructType = ScriptName;

#define SE_SCRIPT_FIELD(FieldName) SE_STRUCT_FIELD(FieldName)

#define SE_END_SCRIPT_REFLECTION() }

} // namespace SE
