// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Engine/Engine.h>
#include <Runtime/Renderer/RHI/RenderingSurface.h>

namespace SE
{

class GameEngine : public Engine
{
public:
    GameEngine() = default;
    virtual ~GameEngine() override = default;
    
    SHOOTER_API virtual bool Initialize() override;
    SHOOTER_API virtual void Shutdown() override;
    SHOOTER_API virtual void Execute() override;

private:
    void OnUpdate(float deltaTime);

    void OnGameWindowResized(const WindowResizedEvent& resizedEvent);

private:
    OwnPtr<Window> m_GameWindow;
    OwnPtr<RenderingSurface> m_GameRenderingSurface;
};

SHOOTER_API extern GameEngine* g_GameEngine;

}
