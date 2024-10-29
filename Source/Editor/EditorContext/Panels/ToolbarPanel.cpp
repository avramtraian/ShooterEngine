/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Assertions.h>
#include <Core/String/String.h>
#include <EditorContext/EditorCamera.h>
#include <EditorContext/Panels/ToolbarPanel.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace SE
{

bool ToolbarPanel::initialize()
{
    m_scene_play_state = ScenePlayState::Edit;
    m_scene_camera_mode = SceneCameraMode::Editor;
    return true;
}

void ToolbarPanel::shutdown()
{
    m_scene_play_state = ScenePlayState::Unknown;
    m_scene_camera_mode = SceneCameraMode::Unknown;
}

void ToolbarPanel::on_update(float delta_time)
{}

void ToolbarPanel::on_render_imgui()
{
    ImGui::Begin("Toolbar");

    draw_save_scene_button();

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    draw_scene_play_state_toggles();

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    draw_scene_camera_mode_toggle();

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    draw_editor_camera_options();

    ImGui::End();
}

void ToolbarPanel::set_editor_camera_controller_context(EditorCameraController* editor_camera_controller_context)
{
    SE_ASSERT(editor_camera_controller_context != nullptr);
    m_editor_camera_controller_context = editor_camera_controller_context;
}

void ToolbarPanel::clear_editor_camera_controller_context()
{
    m_editor_camera_controller_context = nullptr;
}

void ToolbarPanel::set_scene_play_state(ScenePlayState scene_play_state)
{
    const ScenePlayState old_scene_play_state = m_scene_play_state;
    m_scene_play_state = scene_play_state;
    if (m_scene_play_state != old_scene_play_state)
        dispatch_on_scene_play_state_changed_callbacks(old_scene_play_state);
}

void ToolbarPanel::set_scene_camera_mode(SceneCameraMode scene_camera_mode)
{
    const SceneCameraMode old_scene_camera_mode = m_scene_camera_mode;
    m_scene_camera_mode = scene_camera_mode;
    if (m_scene_camera_mode != old_scene_camera_mode)
        dispatch_on_scene_camera_mode_changed_callbacks(old_scene_camera_mode);
}

static float get_toolbar_button_height()
{
    const float toolbar_button_max_height = 50.0F;
    const float available_content_region_height = ImGui::GetContentRegionAvail().y;
    const float toolbar_button_height = Math::min(toolbar_button_max_height, available_content_region_height);
    return toolbar_button_height;
}

void ToolbarPanel::draw_save_scene_button()
{
    const float toolbar_button_height = get_toolbar_button_height();
    if (ImGui::Button("Save", { toolbar_button_height, toolbar_button_height }))
    {
        // Dispatch callbacks.
        dispatch_on_save_scene_button_released_callbacks();
    }
}

void ToolbarPanel::draw_scene_play_state_toggles()
{
    const float toolbar_button_height = get_toolbar_button_height();

    String play_edit_toggle_button_label;
    switch (m_scene_play_state)
    {
        case ScenePlayState::Unknown: play_edit_toggle_button_label = "Unknown"sv; break;
        case ScenePlayState::Edit: play_edit_toggle_button_label = "Play"sv; break;
        case ScenePlayState::Play: play_edit_toggle_button_label = "Stop"sv; break;
        case ScenePlayState::PlayPaused: play_edit_toggle_button_label = "Stop"sv; break;
    }

    if (ImGui::Button(play_edit_toggle_button_label.characters(), { toolbar_button_height, toolbar_button_height }))
    {
        switch (m_scene_play_state)
        {
            case ScenePlayState::Edit: set_scene_play_state(ScenePlayState::Play); break;
            case ScenePlayState::Play: set_scene_play_state(ScenePlayState::Edit); break;
            case ScenePlayState::PlayPaused: set_scene_play_state(ScenePlayState::Edit); break;
        }
    }

    String pause_toggle_button_label;
    bool is_pause_toggle_button_activated = false;

    switch (m_scene_play_state)
    {
        case ScenePlayState::Unknown:
            pause_toggle_button_label = "Unknown"sv;
            is_pause_toggle_button_activated = false;
            break;

        case ScenePlayState::Edit:
            pause_toggle_button_label = "Pause"sv;
            is_pause_toggle_button_activated = false;
            break;

        case ScenePlayState::Play:
            pause_toggle_button_label = "Pause"sv;
            is_pause_toggle_button_activated = true;
            break;

        case ScenePlayState::PlayPaused:
            pause_toggle_button_label = "Resume"sv;
            is_pause_toggle_button_activated = true;
            break;
    }

    if (!is_pause_toggle_button_activated)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5F);
    }

    ImGui::SameLine();
    if (ImGui::Button(pause_toggle_button_label.characters(), { toolbar_button_height, toolbar_button_height }))
    {
        switch (m_scene_play_state)
        {
            case ScenePlayState::Play: set_scene_play_state(ScenePlayState::PlayPaused); break;
            case ScenePlayState::PlayPaused: set_scene_play_state(ScenePlayState::Play); break;
        }
    }

    if (!is_pause_toggle_button_activated)
    {
        ImGui::PopStyleVar();
        ImGui::PopItemFlag();
    }
}

void ToolbarPanel::draw_scene_camera_mode_toggle()
{
    const float toolbar_button_height = get_toolbar_button_height();

    String camera_mode_toggle_button_label;
    switch (m_scene_camera_mode)
    {
        case SceneCameraMode::Unknown: camera_mode_toggle_button_label = "Unknown"sv; break;
        case SceneCameraMode::Game: camera_mode_toggle_button_label = "Editor"sv; break;
        case SceneCameraMode::Editor: camera_mode_toggle_button_label = "Game"sv; break;
    }

    if (ImGui::Button(camera_mode_toggle_button_label.characters(), { toolbar_button_height, toolbar_button_height }))
    {
        switch (m_scene_camera_mode)
        {
            case SceneCameraMode::Game: set_scene_camera_mode(SceneCameraMode::Editor); break;
            case SceneCameraMode::Editor: set_scene_camera_mode(SceneCameraMode::Game); break;
        }
    }
}

void ToolbarPanel::draw_editor_camera_options()
{
    if (has_editor_camera_controller_context())
    {
        const float toolbar_button_height = get_toolbar_button_height();

        if (ImGui::Button("##EditorCameraOptions", { toolbar_button_height, toolbar_button_height }))
            ImGui::OpenPopup("EditorCameraOptionsPopup");

        if (ImGui::BeginPopup("EditorCameraOptionsPopup"))
        {
            ImGui::SeparatorText("Editor Camera Options");
            const EditorCameraController default_controller;

            const float reset_button_size = ImGui::GetTextLineHeight() + 2.0F * GImGui->Style.FramePadding.y;

            // Movement Speed Factor.
            {
                const float value_step = 0.1F;
                const float min_value = 0.1F;
                const float max_value = 20.0F;

                float movement_speed = m_editor_camera_controller_context->movement_speed_factor;

                if (movement_speed != default_controller.movement_speed_factor)
                {
                    if (ImGui::Button("##MovementSpeedReset", { reset_button_size, reset_button_size }))
                    {
                        // Reset the movement speed.
                        movement_speed = default_controller.movement_speed_factor;
                    }

                    ImGui::SameLine();
                }

                ImGui::DragFloat("Movement Speed", &movement_speed, value_step, min_value, max_value);
                m_editor_camera_controller_context->movement_speed_factor = movement_speed;
            }

            // Movement Speed Boost Factor.
            {
                const float value_step = 0.1F;
                const float min_value = 0.1F;
                const float max_value = 20.0F;

                float movement_speed_boost = m_editor_camera_controller_context->speed_boost_factor;

                if (movement_speed_boost != default_controller.speed_boost_factor)
                {
                    if (ImGui::Button("##MovementSpeedBoostReset", { reset_button_size, reset_button_size }))
                    {
                        // Reset the movement speed.
                        movement_speed_boost = default_controller.speed_boost_factor;
                    }

                    ImGui::SameLine();
                }

                ImGui::DragFloat("Movement Speed Boost", &movement_speed_boost, value_step, min_value, max_value);
                m_editor_camera_controller_context->speed_boost_factor = movement_speed_boost;
            }

            // Mouse Sensitivity Factor.
            {
                const float value_step = 0.1F;
                const float min_value = 0.1F;
                const float max_value = 20.0F;

                float mouse_sensitivity = m_editor_camera_controller_context->first_person_mouse_sensitivity_factor;

                if (mouse_sensitivity != default_controller.first_person_mouse_sensitivity_factor)
                {
                    if (ImGui::Button("##MouseSensitibityReset", { reset_button_size, reset_button_size }))
                    {
                        // Reset the movement speed.
                        mouse_sensitivity = default_controller.first_person_mouse_sensitivity_factor;
                    }

                    ImGui::SameLine();
                }

                ImGui::DragFloat("Mouse Sensitivity", &mouse_sensitivity, value_step, min_value, max_value);
                m_editor_camera_controller_context->first_person_mouse_sensitivity_factor = mouse_sensitivity;
            }

            ImGui::EndPopup();
        }
    }
}

void ToolbarPanel::dispatch_on_scene_play_state_changed_callbacks(ScenePlayState old_scene_play_state)
{
    for (auto& callback : m_on_scene_play_state_changed_callbacks)
        callback(old_scene_play_state, m_scene_play_state);
}

void ToolbarPanel::dispatch_on_scene_camera_mode_changed_callbacks(SceneCameraMode old_scene_camera_mode)
{
    for (auto& callback : m_on_scene_camera_mode_changed_callbacks)
        callback(old_scene_camera_mode, m_scene_camera_mode);
}

void ToolbarPanel::dispatch_on_save_scene_button_released_callbacks()
{
    for (auto& callback : m_on_save_scene_button_released)
        callback();
}

} // namespace SE
