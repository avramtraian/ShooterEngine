// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/UUID.h>
#include <Runtime/Scene/System.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION SYSTEM.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Constructs a new system at the provided memory address. It overrides any data currently stored there.
// It returns a pointer to the newly created system. Its destructor must be called before releasing the memory block.
using PFN_ConstructSystem = System*(*)(void*);

class ReflectionSystem
{
    SE_MAKE_NONCOPYABLE(ReflectionSystem);
    SE_MAKE_NONMOVABLE(ReflectionSystem);

public:
    ReflectionSystem() = default;
    ~ReflectionSystem() = default;

    NODISCARD ALWAYS_INLINE UUID GetSystemUUID() const { return m_SystemUUID; }
    NODISCARD ALWAYS_INLINE const String& GetName() const { return m_Name; }

    RUNTIME_API void SetSystemUUID(UUID systemUUID);
    RUNTIME_API void SetName(String name);

    RUNTIME_API void SetConstructFunction(PFN_ConstructSystem function);
    RUNTIME_API System* ExecuteConstruct(void* dstMemoryBlock);

private:
    UUID m_SystemUUID { UUID::Invalid() };
    String m_Name;
    PFN_ConstructSystem m_ConstructFunction;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION SYSTEM UUID.
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct ReflectionSystemMetadata
{
    SE_MAKE_NAMESPACE_CLASS(ReflectionSystemMetadata);

public:
    static constexpr bool IsSpecialized { false };
    static constexpr UUID SystemUUID { UUID::Invalid() };
};

#define SE_REFLECTION_SYSTEM_METADATA(SystemName, systemUUIDValue)  \
    template<>                                                      \
    struct ReflectionSystemMetadata<class SystemName>               \
    {                                                               \
        SE_MAKE_NAMESPACE_CLASS(ReflectionSystemMetadata);          \
    public:                                                         \
        static constexpr bool IsSpecialized { true };               \
        static constexpr UUID SystemUUID { systemUUIDValue };       \
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// REFLECTION SYSTEM MACROS.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SE_BEGIN_SYSTEM_REFLECTION(SystemName)                                                                      \
    {                                                                                                               \
        using Metadata = ReflectionSystemMetadata<SystemName>;                                                      \
        ReflectionSystem& reflectionSystem = SceneReflectionRegistry::CreateSystemFromUUID(Metadata::SystemUUID);   \
        reflectionSystem.SetSystemUUID(Metadata::SystemUUID);                                                       \
        reflectionSystem.SetName(VIEW(#SystemName));                                                                \
        reflectionSystem.SetConstructFunction(                                                                      \
            [](void* dstMemoryBlock) -> System*                                                                     \
            {                                                                                                       \
                return new (dstMemoryBlock) SystemName();                                                           \
            });

#define SE_END_SYSTEM_REFLECTION()                                                                                  \
    }

}
