// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Scene/Reflection/ReflectionComponent.h>
#include <Runtime/Scene/Reflection/ReflectionEnum.h>
#include <Runtime/Scene/Reflection/ReflectionScript.h>
#include <Runtime/Scene/Reflection/ReflectionStruct.h>
#include <Runtime/Scene/Reflection/ReflectionSystem.h>

namespace SE
{

class SceneReflectionRegistry
{
    SE_MAKE_NAMESPACE_CLASS(SceneReflectionRegistry);

public:
    RUNTIME_API static bool Initialize();
    RUNTIME_API static void Shutdown();

public:
    NODISCARD RUNTIME_API static const ReflectionEnum& GetEnumFromUUID(UUID enumUUID);
    NODISCARD RUNTIME_API static const ReflectionStruct& GetStructFromUUID(UUID structUUID);

    NODISCARD RUNTIME_API static const ReflectionComponent& GetComponentFromUUID(UUID componentUUID);
    NODISCARD RUNTIME_API static const ReflectionSystem& GetSystemFromUUID(UUID systemUUID);
    NODISCARD RUNTIME_API static const ReflectionScript& GetScriptFromUUID(UUID scriptUUID);

    NODISCARD RUNTIME_API static Vector<const ReflectionComponent*> GetRegisteredComponents();
    NODISCARD RUNTIME_API static Vector<const ReflectionSystem*> GetRegisteredSystems();
    NODISCARD RUNTIME_API static Vector<const ReflectionScript*> GetRegisteredScripts();

    NODISCARD RUNTIME_API static Vector<const ReflectionScript*> GetRegisteredScriptsDerivedFrom(UUID baseReflectionScriptUUID);

public:
    NODISCARD RUNTIME_API static ReflectionEnum& CreateEnumFromUUID(UUID enumUUID);
    NODISCARD RUNTIME_API static ReflectionStruct& CreateStructFromUUID(UUID structUUID);

    NODISCARD RUNTIME_API static ReflectionComponent& CreateComponentFromUUID(UUID componentUUID);
    NODISCARD RUNTIME_API static ReflectionSystem& CreateSystemFromUUID(UUID systemUUID);
    NODISCARD RUNTIME_API static ReflectionScript& CreateScriptFromUUID(UUID scriptUUID);
};

} // namespace SE
