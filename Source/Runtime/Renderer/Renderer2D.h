/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/RefPtr.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Memory/Buffer.h>
#include <Renderer/Framebuffer.h>
#include <Renderer/IndexBuffer.h>
#include <Renderer/Pipeline.h>
#include <Renderer/RenderPass.h>
#include <Renderer/Shader.h>
#include <Renderer/VertexBuffer.h>

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
    SHOOTER_API bool initialize(RefPtr<Framebuffer> target_framebuffer);

    SHOOTER_API void shutdown();

    SHOOTER_API void begin_frame();
    SHOOTER_API void end_frame();

    SHOOTER_API void invalidate_target_framebuffer(u32 new_width, u32 new_height);

public:
    SHOOTER_API void submit_quad(Vector2 translation, Vector2 scale, Color4 color);

    SHOOTER_API void submit_quad(Vector2 translation, Vector2 scale, RefPtr<Texture2D> texture, Color4 tint_color = Color4(1));

private:
    bool initialize_quads();
    void shutdown_quads();

    void begin_quad_batch();
    void end_quad_batch();

    void construct_quad(Vector2 translation, Vector2 scale, Color4 color, u32 texture_index);

    // Returns an empty optional if no texture slot is available.
    Optional<u32> find_quad_texture_slot_index(const RefPtr<Texture2D>& texture);

private:
    RefPtr<Framebuffer> m_target_framebuffer;
    Statistics m_statistics;

    RefPtr<Shader> m_quad_shader;
    RefPtr<Pipeline> m_quad_pipeline;
    RefPtr<RenderPass> m_quad_render_pass;
    RefPtr<VertexBuffer> m_quad_vertex_buffer;
    RefPtr<IndexBuffer> m_quad_index_buffer;

    u32 m_max_quads_per_batch { 0 };
    Vector<QuadVertex> m_quad_vertices;

    u32 m_max_quad_textures_per_batch { 0 };
    Vector<RefPtr<Texture2D>> m_quad_textures;
};

} // namespace SE
