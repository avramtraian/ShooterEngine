// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Engine/Engine.h>

namespace SE
{

class GameEngine : public Engine
{
public:
    SHOOTER_API virtual bool initialize() override;
    SHOOTER_API virtual void shutdown() override;
    SHOOTER_API virtual void execute() override;

public:
    GameEngine() = default;
    virtual ~GameEngine() override = default;

private:
    OwnPtr<Window> m_game_window;
};

SHOOTER_API extern GameEngine* g_game_engine;

}
