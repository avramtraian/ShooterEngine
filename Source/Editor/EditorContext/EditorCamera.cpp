/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/Matrix.h>
#include <Core/Math/MatrixTransformations.h>
#include <EditorContext/EditorCamera.h>

namespace SE
{

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

void EditorCamera::set_vertical_fov(float vertical_fov)
{
    m_vertical_fov = vertical_fov;
    m_view_projection_matrix = calculate_view_projection_matrix();
}

void EditorCamera::set_aspect_ratio(float aspect_ratio)
{
    m_aspect_ratio = aspect_ratio;
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
    Optional<float> vertical_fov,
    Optional<float> aspect_ratio,
    Optional<float> clip_near,
    Optional<float> clip_far
)
{
    m_translation = translation.value_or(m_translation);
    m_rotation = rotation.value_or(m_rotation);
    m_vertical_fov = vertical_fov.value_or(m_vertical_fov);
    m_aspect_ratio = aspect_ratio.value_or(m_aspect_ratio);
    m_clip_near = clip_near.value_or(m_clip_near);
    m_clip_far = clip_far.value_or(m_clip_far);

    // Recalculate the view-projection matrix.
    m_view_projection_matrix = calculate_view_projection_matrix();
}

Matrix4 EditorCamera::calculate_view_matrix() const
{
    const Matrix4 inverse_view_matrix = Matrix4::rotate(m_rotation) * Matrix4::translate(m_translation);
    return Matrix4::inverse(inverse_view_matrix);
}

Matrix4 EditorCamera::calculate_projection_matrix() const
{
    const Matrix4 projection_matrix = Matrix4::perspective(m_vertical_fov, m_aspect_ratio, m_clip_near, m_clip_far);
    return projection_matrix;
}

Matrix4 EditorCamera::calculate_view_projection_matrix() const
{
    const Matrix4 view_matrix = calculate_view_matrix();
    const Matrix4 projection_matrix = calculate_projection_matrix();
    return view_matrix * projection_matrix;
}

} // namespace SE
