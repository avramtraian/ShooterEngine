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
    ENGINE_API static bool Initialize();
    ENGINE_API static void Shutdown();

public:
    NODISCARD ENGINE_API static const ReflectionEnum& GetEnumFromUUID(UUID enumUUID);
    NODISCARD ENGINE_API static const ReflectionStruct& GetStructFromUUID(UUID structUUID);

    NODISCARD ENGINE_API static const ReflectionComponent& GetComponentFromUUID(UUID componentUUID);
    NODISCARD ENGINE_API static const ReflectionSystem& GetSystemFromUUID(UUID systemUUID);
    NODISCARD ENGINE_API static const ReflectionScript& GetScriptFromUUID(UUID scriptUUID);

public:
    NODISCARD ENGINE_API static ReflectionEnum& CreateEnumFromUUID(UUID enumUUID);
    NODISCARD ENGINE_API static ReflectionStruct& CreateStructFromUUID(UUID structUUID);

    NODISCARD ENGINE_API static ReflectionComponent& CreateComponentFromUUID(UUID componentUUID);
    NODISCARD ENGINE_API static ReflectionSystem& CreateSystemFromUUID(UUID systemUUID);
    NODISCARD ENGINE_API static ReflectionScript& CreateScriptFromUUID(UUID scriptUUID);
};

}
