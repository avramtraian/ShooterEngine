// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Log.h>
#include <Runtime/Scene/Reflection/Fields/FieldTypeFinder.h>
#include <Runtime/Scene/Reflection/SceneReflectionRegistry.h>

namespace SE
{

struct SceneReflectionRegistryData
{
    // Raw (C++) types.
    HashMap<UUID, OwnPtr<ReflectionEnum>> Enums;
    HashMap<UUID, OwnPtr<ReflectionStruct>> Structs;

    // Engine (ECS) types.
    HashMap<UUID, OwnPtr<ReflectionComponent>> Components;
    HashMap<UUID, OwnPtr<ReflectionSystem>> Systems;
    HashMap<UUID, OwnPtr<ReflectionScript>> Scripts;
};
static SceneReflectionRegistryData* s_SceneReflectionRegistry;

bool SceneReflectionRegistry::Initialize()
{
    if (s_SceneReflectionRegistry)
    {
        SE_LOG_ERROR("The scene reflection registry was already initialized!");
        return false;
    }
    s_SceneReflectionRegistry = new SceneReflectionRegistryData();

    return true;
}

void SceneReflectionRegistry::Shutdown()
{
    if (!s_SceneReflectionRegistry)
    {
        SE_LOG_WARN("The scene reflection registry was already shut down or was never initialized!");
        return;
    }

    delete s_SceneReflectionRegistry;
    s_SceneReflectionRegistry = nullptr;
}

const ReflectionEnum& SceneReflectionRegistry::GetEnumFromUUID(UUID enumUUID)
{
    auto reflectionEnum = s_SceneReflectionRegistry->Enums.GetIfExists(enumUUID);
    SE_ASSERT(reflectionEnum.HasValue());
    return *reflectionEnum.Value();
}

const ReflectionStruct& SceneReflectionRegistry::GetStructFromUUID(UUID structUUID)
{
    auto reflectionStruct = s_SceneReflectionRegistry->Structs.GetIfExists(structUUID);
    SE_ASSERT(reflectionStruct.HasValue());
    return *reflectionStruct.Value();
}

const ReflectionComponent& SceneReflectionRegistry::GetComponentFromUUID(UUID componentUUID)
{
    auto reflectionComponent = s_SceneReflectionRegistry->Components.GetIfExists(componentUUID);
    SE_ASSERT(reflectionComponent.HasValue());
    return *reflectionComponent.Value();
}

const ReflectionSystem& SceneReflectionRegistry::GetSystemFromUUID(UUID systemUUID)
{
    auto reflectionSystem = s_SceneReflectionRegistry->Systems.GetIfExists(systemUUID);
    SE_ASSERT(reflectionSystem.HasValue());
    return *reflectionSystem.Value();
}

const ReflectionScript& SceneReflectionRegistry::GetScriptFromUUID(UUID scriptUUID)
{
    auto reflectionScript = s_SceneReflectionRegistry->Scripts.GetIfExists(scriptUUID);
    SE_ASSERT(reflectionScript.HasValue());
    return *reflectionScript.Value();
}

ReflectionEnum& SceneReflectionRegistry::CreateEnumFromUUID(UUID enumUUID)
{
    if (s_SceneReflectionRegistry->Enums.Contains(enumUUID))
    {
        SE_LOG_ERROR("Reflection enum with UUID '%d' was already created!", enumUUID.GetValue());
        return *s_SceneReflectionRegistry->Enums.At(enumUUID);
    }

    return *s_SceneReflectionRegistry->Enums.Add(enumUUID, CreateOwn<ReflectionEnum>());
}

ReflectionStruct& SceneReflectionRegistry::CreateStructFromUUID(UUID structUUID)
{
    if (s_SceneReflectionRegistry->Structs.Contains(structUUID))
    {
        SE_LOG_ERROR("Reflection struct with UUID '%d' was already created!", structUUID.GetValue());
        return *s_SceneReflectionRegistry->Structs.At(structUUID);
    }

    return *s_SceneReflectionRegistry->Structs.Add(structUUID, CreateOwn<ReflectionStruct>());
}

ReflectionComponent& SceneReflectionRegistry::CreateComponentFromUUID(UUID componentUUID)
{
    if (s_SceneReflectionRegistry->Components.Contains(componentUUID))
    {
        SE_LOG_ERROR("Reflection enum with UUID '%d' was already created!", componentUUID.GetValue());
        return *s_SceneReflectionRegistry->Components.At(componentUUID);
    }

    return *s_SceneReflectionRegistry->Components.Add(componentUUID, CreateOwn<ReflectionComponent>());
}

ReflectionSystem& SceneReflectionRegistry::CreateSystemFromUUID(UUID systemUUID)
{
    if (s_SceneReflectionRegistry->Systems.Contains(systemUUID))
    {
        SE_LOG_ERROR("Reflection system with UUID '%d' was already created!", systemUUID.GetValue());
        return *s_SceneReflectionRegistry->Systems.At(systemUUID);
    }

    return *s_SceneReflectionRegistry->Systems.Add(systemUUID, CreateOwn<ReflectionSystem>());
}

ReflectionScript& SceneReflectionRegistry::CreateScriptFromUUID(UUID scriptUUID)
{
    if (s_SceneReflectionRegistry->Scripts.Contains(scriptUUID))
    {
        SE_LOG_ERROR("Reflection script with UUID '%d' was already created!", scriptUUID.GetValue());
        return *s_SceneReflectionRegistry->Scripts.At(scriptUUID);
    }

    return *s_SceneReflectionRegistry->Scripts.Add(scriptUUID, CreateOwn<ReflectionScript>());
}

}
