// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Engine/Engine.h>

namespace SE
{

class GameEngine : public Engine
{
public:
    GameEngine() = default;
    virtual ~GameEngine() override = default;
    
    RUNTIME_API virtual bool Initialize() override;
    RUNTIME_API virtual void Shutdown() override;
    RUNTIME_API virtual void Execute() override;

private:
    void OnUpdate(float deltaTime);

private:
    RefPtr<Window> m_GameWindow;
};

RUNTIME_API extern GameEngine* g_GameEngine;

}
