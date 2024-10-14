/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/HashMap.h>
#include <Core/Containers/OwnPtr.h>
#include <Core/Log.h>
#include <Renderer/Renderer.h>
#include <Renderer/RendererAPI.h>
#include <Renderer/RendererInterface.h>

namespace SE
{

struct RendererData
{
    OwnPtr<RendererInterface> renderer_interface;
    HashMap<void*, OwnPtr<RenderingContext>> context_table;
    RenderingContext* active_rendering_context = nullptr;

    RefPtr<Texture2D> white_texture;
    RefPtr<Texture2D> black_texture;
};

static OwnPtr<RendererData> s_renderer;

bool Renderer::initialize()
{
    if (s_renderer.is_valid())
        return false;
    s_renderer = create_own<RendererData>();

    RendererAPI renderer_api = get_recommended_renderer_api_for_current_platform();
    set_current_renderer_api(renderer_api);

    s_renderer->renderer_interface = RendererInterface::create();
    if (!s_renderer->renderer_interface->initialize())
    {
        SE_LOG_ERROR("Failed to create the renderer interface!");
        return false;
    }

    {
        const u8 white_texture_data[] = { 0xFF, 0xFF, 0xFF, 0xFF };

        Texture2DDescription white_texture_description = {};
        white_texture_description.width = 1;
        white_texture_description.height = 1;
        white_texture_description.format = ImageFormat::RGBA8;
        white_texture_description.data = ReadonlyByteSpan(white_texture_data, sizeof(white_texture_data));
        s_renderer->white_texture = Texture2D::create(white_texture_description);
    }

    {
        const u8 black_texture_data[] = { 0x00, 0x00, 0x00, 0xFF };

        Texture2DDescription black_texture_description = {};
        black_texture_description.width = 1;
        black_texture_description.height = 1;
        black_texture_description.format = ImageFormat::RGBA8;
        black_texture_description.data = ReadonlyByteSpan(black_texture_data, sizeof(black_texture_data));
        s_renderer->black_texture = Texture2D::create(black_texture_description);
    }

    return true;
}

void Renderer::shutdown()
{
    if (!s_renderer.is_valid())
        return;

    s_renderer->white_texture.release();
    s_renderer->black_texture.release();

    s_renderer->context_table.clear();

    s_renderer->renderer_interface->shutdown();
    s_renderer->renderer_interface.release();

    s_renderer.release();
}

bool Renderer::is_initialized()
{
    return s_renderer.is_valid();
}

bool Renderer::create_context_for_window(Window* window)
{
    OwnPtr<RenderingContext>& context = s_renderer->context_table.get_or_add(window->get_native_handle());
    context = RenderingContext::create(window);
    return context.is_valid();
}

RenderingContext* Renderer::get_rendering_context_for_window(Window* window)
{
    Optional<OwnPtr<RenderingContext>&> context = s_renderer->context_table.get_if_exists(window->get_native_handle());
    return context.has_value() ? context.value().get() : nullptr;
}

RenderingContext* Renderer::get_active_context()
{
    return s_renderer->active_rendering_context;
}

void Renderer::set_active_context(RenderingContext* context)
{
    s_renderer->active_rendering_context = context;
}

void Renderer::begin_frame()
{
    // A rendering context must be bound before beginning a new frame.
    SE_ASSERT(s_renderer->active_rendering_context);
}

void Renderer::end_frame()
{
    // A rendering context must be bound in order to end the frame.
    SE_ASSERT(s_renderer->active_rendering_context);
    s_renderer->renderer_interface->present(s_renderer->active_rendering_context);
}

void Renderer::on_resize(u32 new_width, u32 new_height)
{
    s_renderer->renderer_interface->on_resize(new_width, new_height);

    for (auto it : s_renderer->context_table)
        it.value->invalidate(new_width, new_height);
}

void Renderer::begin_render_pass(RefPtr<RenderPass> render_pass)
{
    s_renderer->renderer_interface->begin_render_pass(render_pass);
}

void Renderer::end_render_pass()
{
    s_renderer->renderer_interface->end_render_pass();
}

void Renderer::draw_indexed(RefPtr<VertexBuffer> vertex_buffer, RefPtr<IndexBuffer> index_buffer, u32 index_count)
{
    s_renderer->renderer_interface->draw_indexed(vertex_buffer, index_buffer, index_count);
}

RefPtr<Texture2D> Renderer::get_white_texture()
{
    SE_ASSERT(s_renderer->white_texture.is_valid());
    return s_renderer->white_texture;
}

RefPtr<Texture2D> Renderer::get_black_texture()
{
    SE_ASSERT(s_renderer->black_texture.is_valid());
    return s_renderer->black_texture;
}

} // namespace SE
