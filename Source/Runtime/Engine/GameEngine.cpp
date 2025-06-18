// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/StringView.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Engine/GameEngine.h>

namespace SE
{

SHOOTER_API GameEngine* g_GameEngine = nullptr;

bool GameEngine::Initialize()
{
    if (!InitializeCoreSystems())
    {
        /* There is no point of trying to continue the initialization process if critical
         * systems were not able to be initialized. */
        return false;
    }

    SE_LOG_INFO("All core systems were initialized successfully.");

    m_GameWindow = Window::Create(WindowInfo()
        .SetStartMode(WindowMode::Maximized)
        .SetTitle(VIEW("Shooter Game"))
    );
    if (!m_GameWindow.IsValid())
    {
        /* Failed to initialize the game window. As we don't have a window there is no point
         * in trying to continue the engine initialization process. */
        return false;
    }

    return true;
}

void GameEngine::Shutdown()
{
    /* Destroy the game window. */
    m_GameWindow.Release();

    ShutdownCoreSystems();
}

void GameEngine::Execute()
{
    while (!m_GameWindow->IsRequestedToClose())
    {
        m_GameWindow->PumpMessages();
    }
}

}
