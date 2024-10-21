/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Optional.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector.h>

namespace SE
{

class EditorCamera
{
public:

public:
    NODISCARD ALWAYS_INLINE Vector3 get_translation() const { return m_translation; }
    NODISCARD ALWAYS_INLINE Vector3 get_rotation() const { return m_rotation; }
    NODISCARD ALWAYS_INLINE Matrix4 get_view_projection_matrix() const { return m_view_projection_matrix; }

    void set_translation(Vector3 translation);
    void set_rotation(Vector3 rotation);

    void set_vertical_fov(float vertical_fov);
    void set_aspect_ratio(float aspect_ratio);
    void set_clip_near(float clip_near);
    void set_clip_far(float clip_far);

    void invalidate(
        Optional<Vector3> translation,
        Optional<Vector3> rotation,
        Optional<float> vertical_fov,
        Optional<float> aspect_ratio,
        Optional<float> clip_near,
        Optional<float> clip_far
    );

private:
    NODISCARD Matrix4 calculate_view_matrix() const;
    NODISCARD Matrix4 calculate_projection_matrix() const;
    NODISCARD Matrix4 calculate_view_projection_matrix() const;

private:
    Vector3 m_translation;
    Vector3 m_rotation;
    Matrix4 m_view_projection_matrix;

    float m_vertical_fov { 0.0F };
    float m_aspect_ratio { 0.0F };
    float m_clip_near { 0.0F };
    float m_clip_far { 0.0F };
};

} // namespace SE
