// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Core/Containers/OwnPtr.h>
#include <Runtime/Engine/Engine.h>
#include <Runtime/Renderer/RHI/RenderingSurface.h>

namespace SE
{

class EditorEngine : public Engine
{
public:
    EditorEngine() = default;
    virtual ~EditorEngine() override = default;

    virtual bool Initialize() override;
    virtual void Shutdown() override;
    virtual void Execute() override;

private:
    void OnUpdate(float deltaTime);

private:
    RefPtr<Window> m_EditorWindow;
    RefPtr<RenderingSurface> m_EditorRenderingSurface;
};

extern EditorEngine* GEditorEngine;

}
