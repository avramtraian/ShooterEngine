/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/OwnPtr.h>
#include <Core/API.h>
#include <Engine/Scene/Scene.h>
#include <Renderer/Renderer2D.h>

namespace SE
{

class SceneRenderer
{
public:
    SE_MAKE_NONCOPYABLE(SceneRenderer);
    SE_MAKE_NONMOVABLE(SceneRenderer);
    SceneRenderer() = default;

public:
    SHOOTER_API bool initialize(Scene& in_scene_context, RefPtr<Framebuffer> target_framebuffer);
    SHOOTER_API void shutdown();

    SHOOTER_API void on_resize(u32 new_width, u32 new_height);

    SHOOTER_API bool render(const Matrix4& view_projection_matrix);

private:
    Scene* m_scene_context;
    RefPtr<Framebuffer> m_target_framebuffer;

    OwnPtr<Renderer2D> m_renderer_2d;
};

} // namespace SE
