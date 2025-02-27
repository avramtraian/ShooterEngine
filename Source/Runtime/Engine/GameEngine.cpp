// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/StringView.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Engine/GameEngine.h>

namespace SE
{

SHOOTER_API GameEngine* g_game_engine = nullptr;

bool GameEngine::initialize()
{
    if (!initialize_core_systems())
    {
        /* There is no point of trying to continue the initialization process if critical
         * systems were not able to be initialized. */
        return false;
    }

    SE_LOG_INFO("All core systems were initialized successfully.");

    WindowInfo game_window_info = {};
    game_window_info.mode = WindowMode::Windowed;
    game_window_info.size_x = 1500;
    game_window_info.size_y = 900;

    m_game_window = create_own<Window>();
    if (!m_game_window->initialize(game_window_info))
    {
        /* Failed to initialize the game window. As we don't have a window there is no point
         * in trying to continue the engine initialization process. */
        return false;
    }

    SE_CHECK(false);
    return true;
}

void GameEngine::shutdown()
{
    /* Destroy the game window. */
    m_game_window.release();

    shutdown_core_systems();
}

void GameEngine::execute()
{
    while (!m_game_window->should_close())
    {
        m_game_window->pump_messages();
    }
}

}
