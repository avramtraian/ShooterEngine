// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Engine/Engine.h>
#include <Runtime/Renderer/RHI/RenderingSurface.h>
#include <Runtime/Renderer/SceneRenderer.h>
#include <Runtime/Scene/Scene.h>

namespace SE
{

class EditorEngine : public Engine
{
public:
    EditorEngine()                   = default;
    virtual ~EditorEngine() override = default;

    virtual bool Initialize() override;
    virtual void Shutdown() override;
    virtual void Execute() override;

private:
    void OnUpdate(float deltaTime);
    void OnWindowResized(StrongRefPtr<Window> window, uint32 newWindowSizeX, uint32 newWindowSizeY);

private:
    RefPtr<Window> m_EditorWindow;

    RefPtr<RenderingSurface>    m_EditorRenderingSurface;
    Vector<RefPtr<CommandList>> m_EditorCommandLists;

    RefPtr<Scene>         m_ActiveScene;
    RefPtr<SceneRenderer> m_ActiveSceneRenderer;
};

extern EditorEngine* GEditorEngine;

} // namespace SE
