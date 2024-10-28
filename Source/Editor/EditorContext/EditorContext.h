/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/OwnPtr.h>
#include <Core/String/String.h>
#include <EditorContext/EditorCamera.h>
#include <EditorContext/Panels/ContentBrowserPanel.h>
#include <EditorContext/Panels/EntityInspectorPanel.h>
#include <EditorContext/Panels/SceneHierarchyPanel.h>
#include <EditorContext/Panels/ToolbarPanel.h>
#include <EditorContext/Panels/ViewportPanel.h>
#include <Engine/Application/Events/Event.h>
#include <Engine/Application/Window.h>
#include <Engine/Scene/Reflection/ComponentReflectorRegistry.h>
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

    void on_pre_update(float delta_time);
    void on_update(float delta_time);
    void on_event(const Event& in_event);

public:
    NODISCARD ALWAYS_INLINE OwnPtr<Scene>& get_active_scene() { return m_active_scene; }
    NODISCARD ALWAYS_INLINE const OwnPtr<Scene>& get_active_scene() const { return m_active_scene; }

public:
    NODISCARD BuildConfiguration get_current_build_configuration() const;

    NODISCARD ALWAYS_INLINE String get_engine_root_directory() const { return m_engine_root_directory; }
    NODISCARD ALWAYS_INLINE String get_project_root_directory() const { return m_project_root_directory; }
    NODISCARD ALWAYS_INLINE String get_project_name() const { return m_project_name; }

    NODISCARD String get_project_source_directory() const;
    NODISCARD String get_project_content_directory() const;
    NODISCARD String get_project_binaries_directory(BuildConfiguration) const;

public:
    NODISCARD ALWAYS_INLINE ContentBrowserPanel& get_content_browser_panel() { return m_content_browser_panel; }
    NODISCARD ALWAYS_INLINE EntityInspectorPanel& get_entity_inspector_panel() { return m_entity_inspector_panel; }
    NODISCARD ALWAYS_INLINE SceneHierarchyPanel& get_scene_hierarchy_panel() { return m_scene_hierarchy_panel; }
    NODISCARD ALWAYS_INLINE ViewportPanel& get_viewport_panel() { return m_viewport_panel; }
    NODISCARD ALWAYS_INLINE ToolbarPanel& get_toolbar_panel() { return m_toolbar_panel; }

    NODISCARD ALWAYS_INLINE ScenePlayState get_scene_play_state() const { return m_toolbar_panel.get_scene_play_state(); }

    NODISCARD ALWAYS_INLINE bool is_scene_in_edit_state() const { return get_scene_play_state() == ScenePlayState::Edit; }
    NODISCARD ALWAYS_INLINE bool is_scene_in_play_state() const
    {
        return get_scene_play_state() == ScenePlayState::Play || get_scene_play_state() == ScenePlayState::PlayPaused;
    }

    NODISCARD ALWAYS_INLINE SceneCameraMode get_scene_camera_mode() const { return m_toolbar_panel.get_scene_camera_mode(); }

private:
    void on_update_logic(float delta_time);
    void on_render_imgui();

    void on_scene_play();
    void on_scene_stop();
    void on_scene_pause();
    void on_scene_unpause();

    void on_scene_camera_mode_set_to_editor();
    void on_scene_camera_mode_set_to_game();

private:
    OwnPtr<Window> m_window;

    OwnPtr<Scene> m_active_scene;
    EditorCamera m_editor_camera;
    ComponentReflectorRegistry m_component_reflector_registry;

    RefPtr<Framebuffer> m_scene_framebuffer;
    OwnPtr<SceneRenderer> m_scene_renderer;

    RefPtr<Shader> m_imgui_shader;
    RefPtr<Pipeline> m_imgui_pipeline;
    RefPtr<RenderPass> m_imgui_render_pass;

    ContentBrowserPanel m_content_browser_panel;
    EntityInspectorPanel m_entity_inspector_panel;
    SceneHierarchyPanel m_scene_hierarchy_panel;
    ViewportPanel m_viewport_panel;
    ToolbarPanel m_toolbar_panel;

    String m_engine_root_directory;
    String m_project_root_directory;
    String m_project_name;
};

} // namespace SE
