// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Log.h>
#include <Runtime/CoreObject/GlobalEnvironment.h>
#include <Runtime/Engine/Engine.h>

namespace SE
{

SHOOTER_API Engine* g_Engine = nullptr;

bool Engine::InitializeCoreSystems()
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

    return true;
}

void Engine::ShutdownCoreSystems()
{
    GlobalObjectEnvironment::Shutdown();
    Logger::Shutdown();
}

bool Engine::Initialize()
{
    // Initialize the core engine systems.
    if (!InitializeCoreSystems())
    {
        // There is no point of trying to continue the initialization process if critical
        // systems were not able to be initialized.
        return false;
    }
    SE_LOG_INFO("All core systems were initialized successfully.");

    return true;
}

void Engine::Shutdown()
{
    // Shutdown the core engine systems.
    SE_LOG_INFO("Shutting down the core systems...");
    ShutdownCoreSystems();
}

}
