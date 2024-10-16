/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/HashMap.h>
#include <Engine/Application/Window.h>
#include <Renderer/Renderer.h>
#include <Renderer/RendererAPI.h>
#include <Renderer/RendererInterface.h>
#include <Renderer/RenderingContext.h>

namespace SE
{

struct ContextTableEntry
{
    OwnPtr<RenderingContext> context;
    RefPtr<Framebuffer> swapchain_framebuffer;
};

struct RendererData
{
    OwnPtr<RendererInterface> renderer_interface;
    HashMap<void*, ContextTableEntry> context_table;
    RenderingContext* active_context;

    RefPtr<Texture2D> black_texture;
    RefPtr<Texture2D> white_texture;
};

static RendererData* s_renderer;

bool Renderer::initialize()
{
    if (s_renderer)
        return false;
    s_renderer = new RendererData();

    // Set the rendering API.
    RendererAPI renderer_api = get_recommended_renderer_api_for_current_platform();
    set_current_renderer_api(renderer_api);

    // Create and initialize the renderer interface.
    // This will initialize the actual selected rendering API.
    s_renderer->renderer_interface = RendererInterface::create();
    if (!s_renderer->renderer_interface->initialize())
        return false;

    // Create default textures.

    const u8 black_texture_data[] = { 0x00, 0x00, 0x00, 0xFF };
    const u8 white_texture_data[] = { 0xFF, 0xFF, 0xFF, 0xFF };

    Texture2DDescription black_texture_description = {};
    black_texture_description.width = 1;
    black_texture_description.height = 1;
    black_texture_description.format = ImageFormat::RGBA8;
    black_texture_description.data = ReadonlyByteSpan(black_texture_data, sizeof(black_texture_data));
    s_renderer->black_texture = Texture2D::create(black_texture_description);

    Texture2DDescription white_texture_description = {};
    white_texture_description.width = 1;
    white_texture_description.height = 1;
    white_texture_description.format = ImageFormat::RGBA8;
    white_texture_description.data = ReadonlyByteSpan(white_texture_data, sizeof(white_texture_data));
    s_renderer->white_texture = Texture2D::create(white_texture_description);

    return true;
}

void Renderer::shutdown()
{
    if (s_renderer == nullptr)
        return;

    for (auto it : s_renderer->context_table)
    {
        it.value.swapchain_framebuffer.release();
        it.value.context.release();
    }

    s_renderer->black_texture.release();
    s_renderer->white_texture.release();

    s_renderer->renderer_interface->shutdown();
    s_renderer->renderer_interface.release();

    delete s_renderer;
    s_renderer = nullptr;
}

bool Renderer::is_initialized()
{
    return (s_renderer != nullptr);
}

void Renderer::on_resize(u32 new_width, u32 new_height)
{
    s_renderer->renderer_interface->on_resize(new_width, new_height);

    for (auto it : s_renderer->context_table)
    {
        it.value.context->invalidate(new_width, new_height);

        // NOTE: Invalidating the rendering context will also invalidate the framebuffers that represent
        // swapchain targets, so the following call is not neccessary.
        it.value.swapchain_framebuffer->invalidate(0, 0);
    }
}

RenderingContext* Renderer::create_context_for_window(Window* window)
{
    ContextTableEntry context_entry = {};
    context_entry.context = RenderingContext::create(window);
    context_entry.swapchain_framebuffer = Framebuffer::create({}, *context_entry.context);
    RenderingContext* context = context_entry.context.get();

    SE_ASSERT(!s_renderer->context_table.contains(window->get_native_handle()));
    s_renderer->context_table.add(window->get_native_handle(), move(context_entry));
    return context;
}

void Renderer::destroy_context_for_window(Window* window)
{
    Optional<ContextTableEntry&> context_entry = s_renderer->context_table.get_if_exists(window->get_native_handle());
    SE_ASSERT(context_entry.has_value());

    // NOTE: The swapchain framebuffer must be destroyed before releasing the rendering context.
    // TODO: Actually remove the context table entry instead of just releasing the context and framebuffer.
    context_entry->swapchain_framebuffer.release();
    context_entry->context.release();
}

RenderingContext* Renderer::get_context_for_window(Window* window)
{
    Optional<ContextTableEntry&> context_entry = s_renderer->context_table.get_if_exists(window->get_native_handle());
    SE_ASSERT(context_entry.has_value());
    return context_entry->context.get();
}

void Renderer::set_active_context(RenderingContext* context)
{
    // TODO: Should validate that no operation is scheduled to run on the old context.
    s_renderer->active_context = context;
}

RenderingContext* Renderer::get_active_context()
{
    return s_renderer->active_context;
}

RefPtr<Framebuffer> Renderer::get_swapchain_framebuffer(RenderingContext* context)
{
    for (auto it : s_renderer->context_table)
    {
        if (it.value.context.get() == context)
            return it.value.swapchain_framebuffer;
    }

    return {};
}

void Renderer::begin_frame()
{
    // No context is currently active.
    SE_ASSERT(s_renderer->active_context != nullptr);
}

void Renderer::end_frame()
{
    RenderingContext* active_context = Renderer::get_active_context();
    s_renderer->renderer_interface->present(active_context);
}

void Renderer::begin_render_pass(RefPtr<RenderPass> render_pass)
{
    s_renderer->renderer_interface->begin_render_pass(move(render_pass));
}

void Renderer::end_render_pass()
{
    s_renderer->renderer_interface->end_render_pass();
}

RendererDevice Renderer::get_device()
{
    return s_renderer->renderer_interface->get_renderer_device();
}

void Renderer::draw_indexed(RefPtr<VertexBuffer> vertex_buffer, RefPtr<IndexBuffer> index_buffer, u32 indices_count)
{
    s_renderer->renderer_interface->draw_indexed(vertex_buffer, index_buffer, indices_count);
}

RefPtr<Texture2D> Renderer::get_black_texture()
{
    return s_renderer->black_texture;
}

RefPtr<Texture2D> Renderer::get_white_texture()
{
    return s_renderer->white_texture;
}

} // namespace SE
