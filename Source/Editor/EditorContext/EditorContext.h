/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/OwnPtr.h>
#include <Core/Containers/String.h>
#include <Engine/Application/Event.h>
#include <Engine/Application/Window.h>
#include <Engine/Scene/Scene.h>
#include <Renderer/Pipeline.h>
#include <Renderer/RenderPass.h>
#include <Renderer/SceneRenderer.h>

namespace SE
{

enum class BuildConfiguration : u8
{
    EditorDebug,
    EditorDevelopment,
    GameDebug,
    GameDevelopment,
    GameShipping,
};

class EditorContext
{
public:
    bool pre_initialize();
    bool initialize();
    bool post_initialize();

    void shutdown();

    void on_update(float delta_time);
    void on_event(const Event& in_event);

public:
    NODISCARD BuildConfiguration get_current_build_configuration() const;

    NODISCARD ALWAYS_INLINE String get_engine_root_directory() const { return m_engine_root_directory; }
    NODISCARD ALWAYS_INLINE String get_project_root_directory() const { return m_project_root_directory; }
    NODISCARD ALWAYS_INLINE String get_project_name() const { return m_project_name; }

    NODISCARD String get_project_source_directory() const;
    NODISCARD String get_project_content_directory() const;
    NODISCARD String get_project_binaries_directory(BuildConfiguration) const;

private:
    static void window_event_handler(const Event& in_event);

    void on_update_logic(float delta_time);
    void on_render_imgui();

private:
    OwnPtr<Scene> m_active_scene;
    OwnPtr<Window> m_window;

    RefPtr<Framebuffer> m_scene_framebuffer;
    OwnPtr<SceneRenderer> m_scene_renderer;

    RefPtr<Shader> m_imgui_shader;
    RefPtr<Pipeline> m_imgui_pipeline;
    RefPtr<RenderPass> m_imgui_render_pass;

    String m_engine_root_directory;
    String m_project_root_directory;
    String m_project_name;
};

} // namespace SE