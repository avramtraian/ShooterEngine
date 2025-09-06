// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Log.h>
#include <Runtime/CoreObject/GlobalEnvironment.h>
#include <Runtime/Engine/Engine.h>

namespace SE
{

SHOOTER_API Engine* g_Engine = nullptr;

bool Engine::Initialize()
{
    if (!Logger::Initialize())
    {
        return false;
    }

    if (!GlobalObjectEnvironment::Initialize())
    {
        SE_LOG_ERROR("Failed to initialize the global object environment! Aborting.");
        return false;
    }

    SE_LOG_INFO("All core engine systems were initialized successfully.");
    return true;
}

void Engine::Shutdown()
{
    SE_LOG_INFO("Shutting down the core engine systems...");
    
    GlobalObjectEnvironment::Shutdown();
    Logger::Shutdown();
}

}
