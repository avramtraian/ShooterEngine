// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/CoreObject/GlobalEnvironment.h>
#include <Runtime/CoreObject/Pointers/SObjectPtr.h>
#include <Runtime/CoreObject/Reflection/Class.h>

namespace SE
{

//
// Generates functions and static member variables that the engine expects from a structure
// integrated in the CoreObject reflection and serialization systems.
// All classes derived (indirectly) from the 'Object' class should place this macro inside
// their body definition.
//
#define SE_GENERATE_OBJECT_BODY(ObjectClassName, ParentObjectClassName)                                                                 \
    public:                                                                                                                             \
        NODISCARD FORCEINLINE static SObjectPtr<ObjectClass> GetStaticClass()                                                           \
        {                                                                                                                               \
            return GlobalObjectEnvironment::FindOrCreateObjectClassByName(VIEW(#ObjectClassName));                                      \
        }                                                                                                                               \
        NODISCARD FORCEINLINE virtual SObjectPtr<ObjectClass> GetClass() const override { return ObjectClassName::GetStaticClass(); }   \
        friend void RegisterClasses();                                                                                                  \
        using Super = ParentObjectClassName;                                                                                            \
        ObjectClassName();                                                                                                              \
        virtual ~ObjectClassName() override = default;                                                                                  \
    private:

//
// Generates functions and static member variables that the engine expects from a structure
// integrated in the CoreObject reflection and serialization systems.
// All structures exposed to the reflection and serialization systems should place this macro
// inside their body defintion.
//
#define SE_GENERATE_STRUCT_BODY(StructName)                                                     \
    public:                                                                                     \
        static constexpr void StructTag() {}                                                    \
        FORCEINLINE NODISCARD static StringView GetStaticName() { return VIEW(#StructName); }   \
        static constexpr bool HasParent = false;                                                \
        static const inline StringView ParentName = {};                                         \
    public:

//
// Same as 'SE_GENERATE_STRUCT_BODY', except it also provides static runtime informtion regarding
// the parent structure type.
// All structures exposed to the reflection and serialization systems should place this macro
// inside their body defintion.
//
#define SE_GENERATE_STRUCT_WITH_PARENT_BODY(StructName, ParentStructName)                       \
    public:                                                                                     \
        static constexpr void StructTag() {}                                                    \
        FORCEINLINE NODISCARD static StringView GetStaticName() { return VIEW(#StructName); }   \
        static constexpr bool HasParent = true;                                                 \
        using Parent = ParentStructName;                                                        \
        static const inline StringView ParentName = VIEW(#ParentStructName);                    \
    public:

}
