/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Function.h>
#include <Core/Containers/Vector.h>

namespace SE
{

// Forward declarations.
struct EditorCameraController;

enum class ScenePlayState : u8
{
    Unknown = 0,
    Edit,
    Play,
    PlayPaused,
};

enum class SceneCameraMode : u8
{
    Unknown = 0,
    Game,
    Editor,
};

using PFN_OnScenePlayStateChanged = Function<void(ScenePlayState old_play_state, ScenePlayState new_play_state)>;
using PFN_OnSceneCameraModeChanged = Function<void(SceneCameraMode old_camera_mode, SceneCameraMode new_camera_mode)>;

class ToolbarPanel
{
public:
    bool initialize();
    void shutdown();

    void on_update(float delta_time);
    void on_render_imgui();

public:
    NODISCARD ALWAYS_INLINE EditorCameraController* get_editor_camera_controller_context() const { return m_editor_camera_controller_context; }
    NODISCARD ALWAYS_INLINE bool has_editor_camera_controller_context() const { return (m_editor_camera_controller_context != nullptr); }
    void set_editor_camera_controller_context(EditorCameraController* editor_camera_controller_context);
    void clear_editor_camera_controller_context();

    NODISCARD ALWAYS_INLINE ScenePlayState get_scene_play_state() const { return m_scene_play_state; }
    void set_scene_play_state(ScenePlayState scene_play_state);
    void dispatch_on_scene_play_state_changed_callbacks(ScenePlayState old_scene_play_state);

    ALWAYS_INLINE void add_on_scene_play_changed_callback(PFN_OnScenePlayStateChanged callback)
    {
        SE_ASSERT(callback.has_value());
        m_on_scene_play_state_changed_callbacks.add(move(callback));
    }

    NODISCARD ALWAYS_INLINE SceneCameraMode get_scene_camera_mode() const { return m_scene_camera_mode; }
    void set_scene_camera_mode(SceneCameraMode scene_camera_mode);
    void dispatch_on_scene_camera_mode_changed_callbacks(SceneCameraMode old_scene_camera_mode);

    ALWAYS_INLINE void add_on_scene_camera_mode_changed_callback(PFN_OnSceneCameraModeChanged callback)
    {
        SE_ASSERT(callback.has_value());
        m_on_scene_camera_mode_changed_callbacks.add(move(callback));
    }

private:
    void draw_scene_play_state_toggles();
    void draw_scene_camera_mode_toggle();
    void draw_editor_camera_options();

private:
    EditorCameraController* m_editor_camera_controller_context;

    ScenePlayState m_scene_play_state { ScenePlayState::Unknown };
    Vector<PFN_OnScenePlayStateChanged> m_on_scene_play_state_changed_callbacks;

    SceneCameraMode m_scene_camera_mode { SceneCameraMode::Unknown };
    Vector<PFN_OnSceneCameraModeChanged> m_on_scene_camera_mode_changed_callbacks;
};

} // namespace SE
