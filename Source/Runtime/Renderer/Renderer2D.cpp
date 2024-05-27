/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/Containers/StringBuilder.h"
#include "Engine/Engine.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"

namespace SE
{

bool Renderer2D::initialize(u32 width, u32 height, RefPtr<Framebuffer> target_framebuffer /*= {}*/)
{
    //
    // Target framebuffer.
    //

    m_target_framebuffer = target_framebuffer;
    if (!m_target_framebuffer.is_valid())
    {
        FramebufferInfo framebuffer_info = {};
        // framebuffer_info.width = width;
        // framebuffer_info.height = height;
        // framebuffer_info.attachments.add({ ImageFormat::RGBA8 });
        framebuffer_info.is_swapchain_target = true;
        framebuffer_info.rendering_context = Renderer::get_active_context();
        m_target_framebuffer = Framebuffer::create(framebuffer_info);
    }

    //
    // Shaders.
    //

    const String shaders_directory = StringBuilder::path_join({
        g_engine->get_engine_root_directory().view(),
        "Content/Runtime/Shaders/"sv
    });

    ShaderInfo shader_info = {};
    shader_info.stages =
    {
        ShaderStage(ShaderStageType::Vertex,   shaders_directory + "Renderer2D_Quad_V.hlsl"sv),
        ShaderStage(ShaderStageType::Fragment, shaders_directory + "Renderer2D_Quad_F.hlsl"sv),
    };
    m_shader = Shader::create(shader_info);

    //
    // Pipelines.
    //

    PipelineInfo pipeline_info = {};
    pipeline_info.layout =
    {
        { VertexAttribute::Float2, "POSITION"sv },
        { VertexAttribute::Float4, "COLOR"sv },
    };
    pipeline_info.primitive_topology = PrimitiveTopology::TriangleList;
    pipeline_info.shader = m_shader;
    pipeline_info.front_face = FrontFace::Clockwise;
    pipeline_info.enable_culling = false;
    m_pipeline = Pipeline::create(pipeline_info);

    //
    // Render passes.
    //

    RenderPassInfo render_pass_info = {};
    render_pass_info.pipeline = m_pipeline;
    render_pass_info.target.framebuffer = m_target_framebuffer;
    render_pass_info.target.clear = true;
    render_pass_info.target.clear_color = { 0.087F, 0.087F, 0.087F };
    m_render_pass = RenderPass::create(render_pass_info);

    //
    // Vertex buffer.
    //

    m_max_quads_per_batch = 8192;
    // NOTE: Each quad requires 4 vertices in order to be rendered.
    const u32 vertices_count = 4 * m_max_quads_per_batch;

    SE_ASSERT(m_vertices_buffer.is_empty());
    m_vertices_buffer.allocate(vertices_count * sizeof(QuadVertex));
    m_vertices_buffer_pointer = m_vertices_buffer.as<QuadVertex>();

    VertexBufferInfo vertex_buffer_info = {};
    vertex_buffer_info.byte_count = vertices_count * sizeof(QuadVertex);
    vertex_buffer_info.update_frequency = VertexBufferUpdateFrequency::High;
    m_vertex_buffer = VertexBuffer::create(vertex_buffer_info);

    // Construct the geometry of a 1x1 quad, centered in the world origin.
    m_quad_default_positions[0] = { -0.5F,  0.5F }; // Top-Left.
    m_quad_default_positions[1] = {  0.5F,  0.5F }; // Top-Right.
    m_quad_default_positions[2] = {  0.5F, -0.5F }; // Bottom-Right.
    m_quad_default_positions[3] = { -0.5F, -0.5F }; // Bottom-Left.

    //
    // Index buffer.
    //

    // NOTE: Each quad requires 6 indices in order to be rendered.
    const u32 indices_count = 6 * m_max_quads_per_batch;

    ScopedBuffer indices_buffer;
    indices_buffer.allocate(indices_count * sizeof(u32));
    u32* buffer_pointer = indices_buffer.as<u32>();

    for (u32 quad_index = 0; quad_index < m_max_quads_per_batch; ++quad_index)
    {
        *buffer_pointer++ = (4 * quad_index) + 0;
        *buffer_pointer++ = (4 * quad_index) + 1;
        *buffer_pointer++ = (4 * quad_index) + 2;
        *buffer_pointer++ = (4 * quad_index) + 2;
        *buffer_pointer++ = (4 * quad_index) + 3;
        *buffer_pointer++ = (4 * quad_index) + 0;
    }

    IndexBufferInfo index_buffer_info = {};
    index_buffer_info.index_type = IndexType::UInt32;
    index_buffer_info.byte_count = indices_buffer.byte_count();
    index_buffer_info.data = indices_buffer.readonly_span();
    m_index_buffer = IndexBuffer::create(index_buffer_info);

    return true;
}

void Renderer2D::shutdown()
{
    m_index_buffer.release();
    m_vertex_buffer.release();
    m_render_pass.release();
    m_pipeline.release();
    m_shader.release();
    m_target_framebuffer.release();
}

void Renderer2D::begin_frame()
{
    // Reset statistics.
    m_statistics.quads_in_current_frame = 0;

    // Begin the 2D render pass.
    Renderer::begin_render_pass(m_render_pass);

    // Begin the first batch.
    begin_batch();
}

void Renderer2D::end_frame()
{
    // End the last batch.
    end_batch();

    // End the 2D render pass.
    Renderer::end_render_pass();
}

void Renderer2D::resize_target_framebuffer(u32 new_width, u32 new_height)
{
    m_target_framebuffer->resize(new_width, new_height);
}

void Renderer2D::submit_quad(float x, float y, float width, float height, Color4 color)
{
    if (m_statistics.quads_in_current_batch >= m_max_quads_per_batch)
    {
        end_batch();
        begin_batch();
    }

    // Top left.
    m_vertices_buffer_pointer->position = {
        m_quad_default_positions[0].x * width + x,
        m_quad_default_positions[0].y * height + y
    };
    m_vertices_buffer_pointer->color = color;
    ++m_vertices_buffer_pointer;

    // Top right.
    m_vertices_buffer_pointer->position = {
        m_quad_default_positions[1].x * width + x,
        m_quad_default_positions[1].y * height + y
    };
    m_vertices_buffer_pointer->color = color;
    ++m_vertices_buffer_pointer;

    // Bottom right.
    m_vertices_buffer_pointer->position = {
        m_quad_default_positions[2].x * width + x,
        m_quad_default_positions[2].y * height + y
    };
    m_vertices_buffer_pointer->color = color;
    ++m_vertices_buffer_pointer;

    // Bottom left.
    m_vertices_buffer_pointer->position = {
        m_quad_default_positions[3].x * width + x,
        m_quad_default_positions[3].y * height + y
    };
    m_vertices_buffer_pointer->color = color;
    ++m_vertices_buffer_pointer;

    // Update statistics.
    m_statistics.quads_in_current_batch++;
    m_statistics.quads_in_current_frame++;
}

void Renderer2D::begin_batch()
{
    // Reset the vertices buffer pointer.
    m_vertices_buffer_pointer = m_vertices_buffer.as<QuadVertex>();

    // Reset statistics.
    m_statistics.quads_in_current_batch = 0;
}

void Renderer2D::end_batch()
{
    // Sanity check regarding number of quads to render in this batch.
    SE_ASSERT_DEBUG((m_vertices_buffer_pointer - m_vertices_buffer.as<QuadVertex>()) == (m_statistics.quads_in_current_batch * 4));

    // Update the vertex buffer.
    if (m_statistics.quads_in_current_batch > 0)
    {
        const u32 quad_vertices_count = m_statistics.quads_in_current_batch * 4;
        m_vertex_buffer->update_data(m_vertices_buffer.readonly_span().slice(0, quad_vertices_count * sizeof(QuadVertex)));
    }

    // Issue the actual draw command.
    Renderer::draw_indexed(m_vertex_buffer, m_index_buffer, m_statistics.quads_in_current_batch * 6);
}

} // namespace SE
