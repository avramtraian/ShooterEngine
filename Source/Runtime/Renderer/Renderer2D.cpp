/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Asset/AssetManager.h>
#include <Asset/TextureAsset.h>
#include <Core/Containers/StringBuilder.h>
#include <Core/FileSystem/FileSystem.h>
#include <Engine/Engine.h>
#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>

namespace SE
{

bool Renderer2D::initialize(RefPtr<Framebuffer> target_framebuffer)
{
    m_target_framebuffer = move(target_framebuffer);
    return initialize_quads();
}

bool Renderer2D::initialize(u32 target_framebuffer_width, u32 target_framebuffer_height)
{
    FramebufferDescription target_framebuffer_description = {};
    target_framebuffer_description.width = target_framebuffer_width;
    target_framebuffer_description.height = target_framebuffer_height;
    target_framebuffer_description.attachments.emplace(ImageFormat::BGRA8);
    m_target_framebuffer = Framebuffer::create(target_framebuffer_description);

    return initialize_quads();
}

void Renderer2D::shutdown()
{
    shutdown_quads();
    m_target_framebuffer.release();
}

void Renderer2D::begin_frame()
{
    m_statistics.quads_in_current_batch = 0;
    m_statistics.quads_in_current_frame = 0;
    m_statistics.quad_textures_in_current_batch = 0;

    begin_quad_batch();
}

void Renderer2D::end_frame()
{
    end_quad_batch();
}

void Renderer2D::invalidate_target_framebuffer(u32 new_width, u32 new_height)
{
    if (m_target_framebuffer->is_swapchain_target())
    {
        // There is no need to invalidate a swapchain target framebuffer.
        return;
    }

    m_target_framebuffer->invalidate(new_width, new_height);
}

void Renderer2D::submit_quad(Vector2 translation, Vector2 scale, Color4 color)
{
    if (m_statistics.quads_in_current_batch == m_max_quads_per_batch)
    {
        end_quad_batch();
        begin_quad_batch();
    }

    RefPtr<Texture2D> white_texture = Renderer::get_white_texture();
    Optional<u32> texture_index = find_quad_texture_slot_index(white_texture);
    if (!texture_index.has_value())
    {
        end_quad_batch();
        begin_quad_batch();
        texture_index = find_quad_texture_slot_index(white_texture);
    }

    SE_DEBUG_ASSERT(texture_index.has_value());
    construct_quad(translation, scale, color, texture_index.value());
}

void Renderer2D::submit_quad(Vector2 translation, Vector2 scale, RefPtr<Texture2D> texture, Color4 tint_color /*= Color4(1)*/)
{
    if (m_statistics.quads_in_current_batch == m_max_quads_per_batch)
    {
        end_quad_batch();
        begin_quad_batch();
    }

    Optional<u32> texture_index = find_quad_texture_slot_index(texture);
    if (!texture_index.has_value())
    {
        end_quad_batch();
        begin_quad_batch();
        texture_index = find_quad_texture_slot_index(texture);
    }

    SE_DEBUG_ASSERT(texture_index.has_value());
    construct_quad(translation, scale, tint_color, texture_index.value());
}

bool Renderer2D::initialize_quads()
{
    m_max_quads_per_batch = 8192;
    m_max_quad_textures_per_batch = 8;

    // Each quad requires 4 vertices in order to be rendered.
    m_quad_vertices.set_count(4 * m_max_quads_per_batch);
    m_quad_textures.set_count(m_max_quad_textures_per_batch);

    m_statistics.quads_in_current_batch = 0;
    m_statistics.quads_in_current_frame = 0;
    m_statistics.quad_textures_in_current_batch = 0;

    //
    // Quad shader.
    //

    ShaderDescription shader_description = {};
    String vertex_shader_source_code;
    String fragment_shader_source_code;

    // The root directory where all engine shaders are stored on disk.
    const String shaders_directory = StringBuilder::path_join({ g_engine->get_engine_root_directory().view(), "Content/Runtime/Shaders"sv });

    {
        FileReader vertex_shader_file_reader;
        vertex_shader_file_reader.open(shaders_directory + "/Renderer2D_Quad_V.hlsl"sv);
        vertex_shader_source_code;
        vertex_shader_file_reader.read_entire_to_string_and_close(vertex_shader_source_code);

        ShaderStageDescription& vertex_stage_description = shader_description.stages.emplace();
        vertex_stage_description.stage = ShaderStage::Vertex;
        vertex_stage_description.source_type = ShaderSourceType::SourceCode;
        vertex_stage_description.source_code = vertex_shader_source_code.view();
    }

    {
        FileReader fragment_shader_file_reader;
        fragment_shader_file_reader.open(shaders_directory + "/Renderer2D_Quad_F.hlsl"sv);
        fragment_shader_source_code;
        fragment_shader_file_reader.read_entire_to_string_and_close(fragment_shader_source_code);

        ShaderStageDescription& fragment_stage_description = shader_description.stages.emplace();
        fragment_stage_description.stage = ShaderStage::Fragment;
        fragment_stage_description.source_type = ShaderSourceType::SourceCode;
        fragment_stage_description.source_code = fragment_shader_source_code.view();
    }

    shader_description.debug_name = "Renderer2D_Quad"sv;
    m_quad_shader = Shader::create(shader_description);

    vertex_shader_source_code.clear();
    fragment_shader_source_code.clear();

    //
    // Quad pipeline.
    //

    PipelineDescription pipeline_description = {};
    pipeline_description.shader = m_quad_shader;
    pipeline_description.vertex_attributes.add({ PipelineVertexAttributeType::Float2, "POSITION"sv });
    pipeline_description.vertex_attributes.add({ PipelineVertexAttributeType::Float4, "COLOR"sv });
    pipeline_description.vertex_attributes.add({ PipelineVertexAttributeType::Float2, "TEXTURE_COORDINATES"sv });
    pipeline_description.vertex_attributes.add({ PipelineVertexAttributeType::UInt1, "TEXTURE_ID"sv });
    pipeline_description.primitive_topology = PipelinePrimitiveTopology::TriangleList;
    pipeline_description.cull_mode = PipelineCullMode::None;

    m_quad_pipeline = Pipeline::create(pipeline_description);

    //
    // Quad render pass.
    //

    RenderPassDescription render_pass_description = {};
    render_pass_description.pipeline = m_quad_pipeline;
    render_pass_description.target_framebuffer = m_target_framebuffer;
    render_pass_description.target_framebuffer_attachments.set_fixed_capacity(m_target_framebuffer->get_attachment_count());
    for (u32 attachment_index = 0; attachment_index < m_target_framebuffer->get_attachment_count(); ++attachment_index)
    {
        RenderPassAttachmentDescription attachment_description = {};
        attachment_description.load_operation = RenderPassAttachmentLoadOperation::Clear;
        attachment_description.clear_color = Color4(1, 0, 0);
        render_pass_description.target_framebuffer_attachments.add(attachment_description);
    }

    m_quad_render_pass = RenderPass::create(render_pass_description);

    m_quad_render_pass->set_input("u_Textures"sv, RenderPassTextureArrayBinding(m_quad_textures.span()));

    //
    // Quad vertex buffer.
    //

    VertexBufferDescription vertex_buffer_description = {};
    vertex_buffer_description.byte_count = static_cast<u32>(m_quad_vertices.count() * sizeof(QuadVertex));
    vertex_buffer_description.update_frequency = VertexBufferUpdateFrequency::High;
    m_quad_vertex_buffer = VertexBuffer::create(vertex_buffer_description);

    //
    // Quad index buffer.
    //

    Buffer indices_buffer;
    // Each quad requires 6 indices in order to be rendered.
    indices_buffer.allocate_new((6 * m_max_quads_per_batch) * sizeof(u32));
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

    IndexBufferDescription index_buffer_description = {};
    index_buffer_description.index_type = IndexType::UInt32;
    index_buffer_description.byte_count = indices_buffer.byte_count();
    index_buffer_description.data = indices_buffer.readonly_byte_span();
    m_quad_index_buffer = IndexBuffer::create(index_buffer_description);
    indices_buffer.release();

    return true;
}

void Renderer2D::shutdown_quads()
{
    m_max_quads_per_batch = 0;
    m_max_quad_textures_per_batch = 0;

    m_quad_vertices.clear_and_shrink();
    m_quad_textures.clear_and_shrink();

    m_quad_index_buffer.release();
    m_quad_vertex_buffer.release();
    m_quad_render_pass.release();
    m_quad_pipeline.release();
    m_quad_shader.release();
}

void Renderer2D::begin_quad_batch()
{
    m_statistics.quads_in_current_batch = 0;
    m_statistics.quad_textures_in_current_batch = 0;

    // Release all textures.
    for (RefPtr<Texture2D>& texture : m_quad_textures)
        texture.release();
}

void Renderer2D::end_quad_batch()
{
    Renderer::begin_render_pass(m_quad_render_pass);

    if (m_statistics.quads_in_current_batch > 0)
    {
        // Upload the vertices to the vertex buffer.
        const u32 vertices_count = 4 * m_statistics.quads_in_current_batch;
        m_quad_vertex_buffer->update_data(m_quad_vertices.slice(0, vertices_count).as<ReadonlyByte>());

        // Update the textures.
        m_quad_render_pass->update_input("u_Textures"sv, m_quad_textures.span());
        m_quad_render_pass->bind_inputs();

        // Each quad requires 6 indices in order to be rendered.
        Renderer::draw_indexed(m_quad_vertex_buffer, m_quad_index_buffer, 6 * m_statistics.quads_in_current_batch);
    }

    Renderer::end_render_pass();
}

void Renderer2D::construct_quad(Vector2 translation, Vector2 scale, Color4 color, u32 texture_index)
{
    SE_ASSERT(m_statistics.quads_in_current_batch < m_max_quads_per_batch);
    SE_ASSERT(texture_index < m_max_quad_textures_per_batch);
    QuadVertex* vertices = m_quad_vertices.elements() + (4 * m_statistics.quads_in_current_batch);

    // Bottom-left vertex.
    vertices[0].position = translation + Vector2(-0.5F * scale.x, -0.5F * scale.y);
    vertices[0].color = color;
    vertices[0].texture_coordinates = Vector2(0, 0);
    vertices[0].texture_id = texture_index;

    // Bottom-right vertex.
    vertices[1].position = translation + Vector2(0.5F * scale.x, -0.5F * scale.y);
    vertices[1].color = color;
    vertices[1].texture_coordinates = Vector2(1, 0);
    vertices[1].texture_id = texture_index;

    // Top-right vertex.
    vertices[2].position = translation + Vector2(0.5F * scale.x, 0.5F * scale.y);
    vertices[2].color = color;
    vertices[2].texture_coordinates = Vector2(1, 1);
    vertices[2].texture_id = texture_index;

    // Top-left vertex.
    vertices[3].position = translation + Vector2(-0.5F * scale.x, 0.5F * scale.y);
    vertices[3].color = color;
    vertices[3].texture_coordinates = Vector2(0, 1);
    vertices[3].texture_id = texture_index;

    m_statistics.quads_in_current_batch++;
    m_statistics.quads_in_current_frame++;
}

Optional<u32> Renderer2D::find_quad_texture_slot_index(const RefPtr<Texture2D>& texture)
{
    for (u32 texture_index = 0; texture_index < m_statistics.quad_textures_in_current_batch; ++texture_index)
    {
        if (m_quad_textures[texture_index].get() == texture.get())
            return texture_index;
    }

    if (m_statistics.quad_textures_in_current_batch == m_max_quad_textures_per_batch)
        return {};

    m_quad_textures[m_statistics.quad_textures_in_current_batch] = texture;
    return (m_statistics.quad_textures_in_current_batch++);
}

} // namespace SE
