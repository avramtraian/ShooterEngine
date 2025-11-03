// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Log.h>
#include <Runtime/Engine/Engine.h>
#include <Runtime/Scene/Reflection/SceneReflectionRegistry.h>

namespace SE
{

RUNTIME_API Engine* g_Engine = nullptr;

bool Engine::Initialize()
{
    if (!Logger::Initialize())
    {
        return false;
    }

    if (!SceneReflectionRegistry::Initialize())
    {
        SE_LOG_ERROR("Failed to initialize the scene reflection registry!")
        return false;
    }
    SE_LOG_INFO("Successfully initialized the scene reflection registry.");

    SE_LOG_INFO("All core engine systems were initialized successfully.");
    return true;
}

void Engine::Shutdown()
{
    SE_LOG_INFO("Shutting down the core engine systems...");
    
    SceneReflectionRegistry::Shutdown();
    Logger::Shutdown();
}

}
