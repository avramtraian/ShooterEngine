/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/Matrix.h>
#include <Core/Math/MatrixTransformations.h>
#include <EditorContext/EditorCamera.h>
#include <Engine/Input/Input.h>

namespace SE
{

void EditorCamera::on_update(float delta_time)
{
    const Orientation orientation = get_orientation();
    Vector3 translation_offset = Vector3(0);
    Vector3 rotation_offset = Vector3(0);

    float speed_boost_factor = 1.0F;
    if (Input::is_key_down(m_controller.speed_boost_key_code))
        speed_boost_factor = m_controller.speed_boost_factor;

    if (Input::is_key_down(m_controller.front_key_code))
    {
        const float speed_factor = m_controller.movement_speed_front * m_controller.movement_speed_factor * speed_boost_factor;
        translation_offset += orientation.forward * speed_factor * delta_time;
    }
    if (Input::is_key_down(m_controller.back_key_code))
    {
        const float speed_factor = m_controller.movement_speed_back * m_controller.movement_speed_factor * speed_boost_factor;
        translation_offset -= orientation.forward * speed_factor * delta_time;
    }
    if (Input::is_key_down(m_controller.right_key_code))
    {
        const float speed_factor = m_controller.movement_speed_right * m_controller.movement_speed_factor * speed_boost_factor;
        translation_offset += orientation.right * speed_factor * delta_time;
    }
    if (Input::is_key_down(m_controller.left_key_code))
    {
        const float speed_factor = m_controller.movement_speed_left * m_controller.movement_speed_factor * speed_boost_factor;
        translation_offset -= orientation.right * speed_factor * delta_time;
    }

    if (Input::is_key_down(m_controller.up_key_code))
    {
        const float speed_factor = m_controller.movement_speed_up * m_controller.movement_speed_factor * speed_boost_factor;
        translation_offset += orientation.up * speed_factor * delta_time;
    }
    if (Input::is_key_down(m_controller.down_key_code))
    {
        const float speed_factor = m_controller.movement_speed_down * m_controller.movement_speed_factor;
        translation_offset -= orientation.up * speed_factor * delta_time;
    }

    const float forward_jump = m_controller.movement_forward_jump * Input::get_mouse_wheel_scroll_offset();
    translation_offset += orientation.forward * forward_jump;

    if (Input::is_mouse_button_down(m_controller.first_person_camera_mouse_button))
    {
        // clang-format off
        const float factor_x =
            m_controller.first_person_mouse_sensitivity_axis_x
            * m_controller.first_person_mouse_sensitivity_factor
            * m_controller.mouse_sensitivity_axis_x
            * m_controller.mouse_sensitivity_factor;
        
        const float factor_y =
            m_controller.first_person_mouse_sensitivity_axis_y
            * m_controller.first_person_mouse_sensitivity_factor
            * m_controller.mouse_sensitivity_axis_y
            * m_controller.mouse_sensitivity_factor;
        // clang-format on

        const Vector2 mouse_offset = Input::get_mouse_offset();
        m_rotation.y += Math::radians(0.15F) * mouse_offset.x * factor_x;
        m_rotation.x -= Math::radians(0.15F) * mouse_offset.y * factor_y;
    }
    else if (Input::is_mouse_button_down(m_controller.pan_camera_mouse_button))
    {
        // clang-format off
        const float factor_x =
            m_controller.pan_mouse_sensitivity_axis_x
            * m_controller.pan_mouse_sensitivity_factor
            * m_controller.mouse_sensitivity_axis_x
            * m_controller.mouse_sensitivity_factor;
        
        const float factor_y =
            m_controller.pan_mouse_sensitivity_axis_y
            * m_controller.pan_mouse_sensitivity_factor
            * m_controller.mouse_sensitivity_axis_y
            * m_controller.mouse_sensitivity_factor;
        // clang-format on

        const Vector2 mouse_offset = Input::get_mouse_offset();
        translation_offset -= 0.005F * orientation.right * mouse_offset.x * factor_x;
        translation_offset -= 0.005F * orientation.up * mouse_offset.y * factor_y;
    }

    m_translation += translation_offset;
    m_rotation += rotation_offset;
    m_view_projection_matrix = calculate_view_projection_matrix();
}

void EditorCamera::set_translation(Vector3 translation)
{
    m_translation = translation;
    m_view_projection_matrix = calculate_view_projection_matrix();
}

void EditorCamera::set_rotation(Vector3 rotation)
{
    m_rotation = rotation;
    m_view_projection_matrix = calculate_view_projection_matrix();
}

void EditorCamera::set_viewport_size(u32 viewport_width, u32 viewport_height)
{
    m_viewport_width = viewport_width;
    m_viewport_height = viewport_height;
    m_view_projection_matrix = calculate_view_projection_matrix();
}

void EditorCamera::set_vertical_fov(float vertical_fov)
{
    m_vertical_fov = vertical_fov;
    m_view_projection_matrix = calculate_view_projection_matrix();
}

void EditorCamera::set_clip_near(float clip_near)
{
    m_clip_near = clip_near;
    m_view_projection_matrix = calculate_view_projection_matrix();
}

void EditorCamera::set_clip_far(float clip_far)
{
    m_clip_far = clip_far;
    m_view_projection_matrix = calculate_view_projection_matrix();
}

void EditorCamera::invalidate(
    Optional<Vector3> translation,
    Optional<Vector3> rotation,
    Optional<u32> viewport_width,
    Optional<u32> viewport_height,
    Optional<float> vertical_fov,
    Optional<float> clip_near,
    Optional<float> clip_far
)
{
    m_translation = translation.value_or(m_translation);
    m_rotation = rotation.value_or(m_rotation);
    m_viewport_width = viewport_width.value_or(m_viewport_width);
    m_viewport_height = viewport_height.value_or(m_viewport_height);
    m_vertical_fov = vertical_fov.value_or(m_vertical_fov);
    m_clip_near = clip_near.value_or(m_clip_near);
    m_clip_far = clip_far.value_or(m_clip_far);

    // Recalculate the view-projection matrix.
    m_view_projection_matrix = calculate_view_projection_matrix();
}

EditorCamera::Orientation EditorCamera::get_orientation() const
{
    const Matrix4 rotation_matrix = Matrix4::rotate(m_rotation);

    // clang-format off
    const Vector4 right_direction   = Vector4(1, 0, 0, 1) * rotation_matrix;
    const Vector4 up_direction      = Vector4(0, 1, 0, 1) * rotation_matrix;
    const Vector4 forward_direction = Vector4(0, 0, 1, 1) * rotation_matrix;
    // clang-format on

    Orientation orientation;
    orientation.right = Vector3(right_direction.x, right_direction.y, right_direction.z);
    orientation.forward = Vector3(forward_direction.x, forward_direction.y, forward_direction.z);
    orientation.up = Vector3(up_direction.x, up_direction.y, up_direction.z);
    return orientation;
}

Matrix4 EditorCamera::calculate_view_matrix() const
{
    const Matrix4 inverse_view_matrix = Matrix4::rotate(m_rotation) * Matrix4::translate(m_translation);
    return Matrix4::inverse(inverse_view_matrix);
}

Matrix4 EditorCamera::calculate_projection_matrix() const
{
    const float aspect_ratio = static_cast<float>(m_viewport_width) / static_cast<float>(m_viewport_height);
    const Matrix4 projection_matrix = Matrix4::perspective(m_vertical_fov, aspect_ratio, m_clip_near, m_clip_far);
    return projection_matrix;
}

Matrix4 EditorCamera::calculate_view_projection_matrix() const
{
    const Matrix4 view_matrix = calculate_view_matrix();
    const Matrix4 projection_matrix = calculate_projection_matrix();
    return view_matrix * projection_matrix;
}

} // namespace SE
