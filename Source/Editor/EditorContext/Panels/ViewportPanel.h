/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Function.h>
#include <Core/Containers/Vector.h>
#include <Core/CoreTypes.h>
#include <Renderer/Framebuffer.h>

namespace SE
{

class ViewportPanel
{
    SE_MAKE_NONCOPYABLE(ViewportPanel);
    SE_MAKE_NONMOVABLE(ViewportPanel);

public:
    ViewportPanel() = default;
    ~ViewportPanel() = default;

    bool initialize();
    void shutdown();

    void on_update(float delta_time);
    void on_render_imgui();

    ALWAYS_INLINE NODISCARD RefPtr<Framebuffer> get_scene_framebuffer_context() const { return m_scene_framebuffer_context; }
    ALWAYS_INLINE void set_scene_framebuffer_context(RefPtr<Framebuffer> context) { m_scene_framebuffer_context = context; }
    ALWAYS_INLINE void clear_scene_framebuffer_context() { m_scene_framebuffer_context.release(); }

    void add_on_viewport_resized_callback(Function<void(u32, u32)> callback) { m_on_viewport_resized_callbacks.emplace(move(callback)); }

private:
    u32 m_viewport_width { 0 };
    u32 m_viewport_height { 0 };
    bool m_is_viewport_size_dirty { false };

    RefPtr<Framebuffer> m_scene_framebuffer_context;

    Vector<Function<void(u32, u32)>> m_on_viewport_resized_callbacks;
};

} // namespace SE
