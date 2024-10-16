/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Engine.h>

namespace SE
{

Engine* g_engine = nullptr;

void Engine::destroy()
{
    if (g_engine)
    {
        delete g_engine;
        g_engine = nullptr;
    }
}

bool Engine::initialize()
{
    m_is_running = true;
    return true;
}

void Engine::shutdown()
{
    m_is_running = false;
}

void Engine::update()
{}

void Engine::exit()
{
    // Stop the engine execution.
    m_is_running = false;
}

} // namespace SE
