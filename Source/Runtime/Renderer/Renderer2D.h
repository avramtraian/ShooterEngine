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
        Vector2 texture_coordinates;
        u32 texture_id;
    };

    struct Statistics
    {
        u32 quads_in_current_batch = 0;
        u32 quads_in_current_frame = 0;
        u32 quad_textures_in_current_batch = 0;
    };

public:
    SHOOTER_API bool initialize(u32 width, u32 height, RefPtr<Framebuffer> target_framebuffer = {});
    SHOOTER_API void shutdown();

    SHOOTER_API void begin_frame();
    SHOOTER_API void end_frame();

    SHOOTER_API void resize_target_framebuffer(u32 new_width, u32 new_height);

    SHOOTER_API void submit_quad(Vector2 position, Vector2 scale, Vector3 rotation, Color4 color, RefPtr<Texture2D> texture = {});

private:
    void begin_batch();
    void end_batch();

    QuadVertex* construct_quad(Vector2 position, Vector2 scale, Color4 color);
    // Returns UINT32_MAX if the texture doesn't exist in the list.
    u32 find_quad_texture_index(const RefPtr<Texture2D>& texture);

private:
    RefPtr<Framebuffer> m_target_framebuffer;
    RefPtr<Shader> m_shader;
    RefPtr<Pipeline> m_pipeline;
    RefPtr<RenderPass> m_render_pass;
    RefPtr<VertexBuffer> m_vertex_buffer;
    RefPtr<IndexBuffer> m_index_buffer;

    u32 m_max_quads_per_batch = 1;
    u32 m_max_quad_textures_per_batch = 1;
    Vector2 m_quad_default_positions[4] = {};
    Buffer m_vertices_buffer;
    QuadVertex* m_vertices_buffer_pointer = nullptr;
    Vector<RefPtr<Texture2D>> m_quad_textures;
    u32 m_quad_texture_count = 0;

    Statistics m_statistics;
};

} // namespace SE
