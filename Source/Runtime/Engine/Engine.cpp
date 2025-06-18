// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Log.h>
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

    return true;
}

void Engine::ShutdownCoreSystems()
{
    Logger::Shutdown();
}

}
