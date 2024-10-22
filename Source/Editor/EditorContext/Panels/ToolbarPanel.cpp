/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Assertions.h>
#include <EditorContext/EditorCamera.h>
#include <EditorContext/Panels/ToolbarPanel.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace SE
{

bool ToolbarPanel::initialize()
{
    return true;
}

void ToolbarPanel::shutdown()
{}

void ToolbarPanel::on_update(float delta_time)
{}

void ToolbarPanel::on_render_imgui()
{
    ImGui::Begin("Toolbar");

    if (has_editor_camera_controller_context())
    {
        const float toolbar_button_max_height = 50.0F;
        const float available_content_region_height = ImGui::GetContentRegionAvail().y;
        const float toolbar_button_height = Math::min(toolbar_button_max_height, available_content_region_height);

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

} // namespace SE
