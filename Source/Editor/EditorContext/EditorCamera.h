/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Optional.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector.h>
#include <Engine/Application/KeyCode.h>

namespace SE
{

//
// Structure that defines the control settings of the editor camera.
// The provided default value for each field will also be the values used if no user
// configuration overrides them.
//
struct EditorCameraController
{
    //================================================================
    // TRANSLATION MANIUPULATION PARAMETERS.
    //================================================================

    KeyCode front_key_code { KeyCode::W };
    KeyCode left_key_code { KeyCode::A };
    KeyCode back_key_code { KeyCode::S };
    KeyCode right_key_code { KeyCode::D };

    KeyCode up_key_code { KeyCode::Space };
    KeyCode down_key_code { KeyCode::Control };

    // Measured in units per second (u/s).
    float movement_speed_front { 2.0F };
    float movement_speed_back { 2.0F };
    float movement_speed_left { 2.0F };
    float movement_speed_right { 2.0F };
    float movement_speed_up { 2.0F };
    float movement_speed_down { 2.0F };
    float movement_speed_factor { 1.0F };

    // Measured in units (u).
    float movement_forward_jump { 0.5F };

    KeyCode speed_boost_key_code { KeyCode::Shift };
    const float speed_boost_factor { 2.0F };

    //================================================================
    // ORIENTATION MANIPULATION PARAMETERS.
    //================================================================

    MouseButton first_person_camera_mouse_button { MouseButton::Right };
    MouseButton pan_camera_mouse_button { MouseButton::Middle };

    float first_person_mouse_sensitivity_axis_x { 1.0F };
    float first_person_mouse_sensitivity_axis_y { 1.0F };
    float first_person_mouse_sensitivity_factor { 1.0F };

    float pan_mouse_sensitivity_axis_x { 1.0F };
    float pan_mouse_sensitivity_axis_y { 1.0F };
    float pan_mouse_sensitivity_factor { 1.0F };

    float mouse_sensitivity_axis_x { 1.0F };
    float mouse_sensitivity_axis_y { 1.0F };
    float mouse_sensitivity_factor { 1.0F };
};

class EditorCamera
{
public:
    struct Orientation
    {
        Vector3 right;
        Vector3 forward;
        Vector3 up;
    };

public:
    void on_update(float delta_time);

public:
    NODISCARD ALWAYS_INLINE Vector3 get_translation() const { return m_translation; }
    NODISCARD ALWAYS_INLINE Vector3 get_rotation() const { return m_rotation; }
    NODISCARD ALWAYS_INLINE Matrix4 get_view_projection_matrix() const { return m_view_projection_matrix; }

    void set_translation(Vector3 translation);
    void set_rotation(Vector3 rotation);

    void set_viewport_size(u32 viewport_width, u32 viewport_height);

    void set_vertical_fov(float vertical_fov);
    void set_clip_near(float clip_near);
    void set_clip_far(float clip_far);

    void invalidate(
        Optional<Vector3> translation,
        Optional<Vector3> rotation,
        Optional<u32> viewport_width,
        Optional<u32> viewport_height,
        Optional<float> vertical_fov,
        Optional<float> clip_near,
        Optional<float> clip_far
    );

    NODISCARD Orientation get_orientation() const;

private:
    NODISCARD Matrix4 calculate_view_matrix() const;
    NODISCARD Matrix4 calculate_projection_matrix() const;
    NODISCARD Matrix4 calculate_view_projection_matrix() const;

private:
    Vector3 m_translation;
    Vector3 m_rotation;
    Matrix4 m_view_projection_matrix;

    u32 m_viewport_width { 0 };
    u32 m_viewport_height { 0 };

    float m_vertical_fov { 0.0F };
    float m_clip_near { 0.0F };
    float m_clip_far { 0.0F };

    EditorCameraController m_controller;
};

} // namespace SE
