// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Log.h>
#include <Runtime/Engine/Engine.h>

namespace SE
{

SHOOTER_API Engine* g_engine = nullptr;

bool Engine::initialize_core_systems()
{
    if (!Logger::initialize())
    {
        return false;
    }

    return true;
}

void Engine::shutdown_core_systems()
{
    Logger::shutdown();
}

}
