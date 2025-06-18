// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Input.h>
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

    SE_LOG_INFO("The primary game window was created successfully.");

    const bool inputInitializeResult = Input::Initialize(InputInfo()
        .AddSourceWindow(m_GameWindow.Get())
    );
    if (!inputInitializeResult)
    {
        /* Failed to initialize the input system. As the user can't input any commands into the game,
        * there is no point in trying to continue the engine initialization. */
        return false;
    }

    SE_LOG_INFO("The input system was initialized successfully.");
    
    return true;
}

void GameEngine::Shutdown()
{
    SE_LOG_INFO("Shutting down the game engine...");

    /* Shutdown the input system. */
    Input::Shutdown();

    /* Destroy the game window. */
    m_GameWindow.Release();

    SE_LOG_INFO("Shutting down the core systems...");
    ShutdownCoreSystems();
}

void GameEngine::Execute()
{
    while (!m_GameWindow->IsRequestedToClose())
    {
        m_GameWindow->PumpMessages();
        OnUpdate();
    }
}

void GameEngine::OnUpdate()
{
    Input::OnUpdate();
    Input::OnPostUpdate();
}

}
