/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Memory/Buffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/RenderPass.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"

namespace SE
{

class Renderer2D
{
public:
    SE_MAKE_NONCOPYABLE(Renderer2D);
    SE_MAKE_NONMOVABLE(Renderer2D);
    Renderer2D() = default;

public:
    struct QuadVertex
    {
        Vector2 position;
        Color4 color;
    };

    struct Statistics
    {
        u32 quads_in_current_batch = 0;
        u32 quads_in_current_frame = 0;
    };

public:
    SHOOTER_API bool initialize(u32 width, u32 height, RefPtr<Framebuffer> target_framebuffer = {});
    SHOOTER_API void shutdown();

    SHOOTER_API void begin_frame();
    SHOOTER_API void end_frame();

    SHOOTER_API void resize_target_framebuffer(u32 new_width, u32 new_height);

    //
    // The 'x' and 'y' parameters represent the position of the center of the quad.
    //
    SHOOTER_API void submit_quad(float x, float y, float width, float height, Color4 color);

private:
    void begin_batch();
    void end_batch();

private:
    RefPtr<Framebuffer> m_target_framebuffer;
    RefPtr<Shader> m_shader;
    RefPtr<Pipeline> m_pipeline;
    RefPtr<RenderPass> m_render_pass;
    RefPtr<VertexBuffer> m_vertex_buffer;
    RefPtr<IndexBuffer> m_index_buffer;

    u32 m_max_quads_per_batch = 1;
    Vector2 m_quad_default_positions[4] = {};
    ScopedBuffer m_vertices_buffer;
    QuadVertex* m_vertices_buffer_pointer = nullptr;

    Statistics m_statistics;
};

} // namespace SE
