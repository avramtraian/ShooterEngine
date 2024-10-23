/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Math/Matrix.h>
#include <Engine/Scene/EntityComponent.h>

namespace SE
{

class CameraComponent : public EntityComponent
{
    SE_ENGINE_ENTITY_COMPONENT(CameraComponent, EntityComponent);

public:
    NODISCARD ALWAYS_INLINE bool is_primary() const { return m_is_primary; }

    NODISCARD ALWAYS_INLINE float get_vertical_field_of_view() const { return m_vertical_field_of_view; }
    NODISCARD ALWAYS_INLINE float get_clip_plane_near() const { return m_clip_plane_near; }
    NODISCARD ALWAYS_INLINE float get_clip_plane_far() const { return m_clip_plane_far; }

    ALWAYS_INLINE void set_vertical_field_of_view(float vertical_field_of_view) { m_vertical_field_of_view = vertical_field_of_view; }
    ALWAYS_INLINE void set_clip_plane_near(float clip_plane_near) { m_clip_plane_near = clip_plane_near; }
    ALWAYS_INLINE void set_clip_plane_far(float clip_plane_far) { m_clip_plane_far = clip_plane_far; }

    NODISCARD SHOOTER_API Matrix4 get_projection_matrix(float aspect_ratio) const;

protected:
    SHOOTER_API virtual void on_update(float delta_time) override;

private:
    bool m_is_primary { true };

    float m_vertical_field_of_view { Math::radians(60.0F) };
    float m_clip_plane_near { 0.001F };
    float m_clip_plane_far { 100000.0F };
};

} // namespace SE
