/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Renderer/Framebuffer.h>
#include <Renderer/IndexBuffer.h>
#include <Renderer/RenderPass.h>
#include <Renderer/RendererDevice.h>
#include <Renderer/Texture.h>
#include <Renderer/VertexBuffer.h>

namespace SE
{

// Forward declarations.
class RenderingContext;
class Window;

class Renderer
{
public:
    SHOOTER_API static bool initialize();
    SHOOTER_API static void shutdown();
    SHOOTER_API static bool is_initialized();

    SHOOTER_API static void on_resize(u32 new_width, u32 new_height);

public:
    SHOOTER_API static RenderingContext* create_context_for_window(Window* window);
    SHOOTER_API static void destroy_context_for_window(Window* window);
    NODISCARD SHOOTER_API static RenderingContext* get_context_for_window(Window* window);

    SHOOTER_API static void set_active_context(RenderingContext* context);
    NODISCARD SHOOTER_API static RenderingContext* get_active_context();

    NODISCARD SHOOTER_API static RefPtr<Framebuffer> get_swapchain_framebuffer(RenderingContext* context);

public:
    SHOOTER_API static void begin_frame();
    SHOOTER_API static void end_frame();

    SHOOTER_API NODISCARD static RendererDevice get_device();

public:
    SHOOTER_API static void begin_render_pass(RefPtr<RenderPass> render_pass);
    SHOOTER_API static void end_render_pass();

public:
    SHOOTER_API static void draw_indexed(RefPtr<VertexBuffer> vertex_buffer, RefPtr<IndexBuffer> index_buffer, u32 indices_count);

public:
    NODISCARD SHOOTER_API static RefPtr<Texture2D> get_black_texture();
    NODISCARD SHOOTER_API static RefPtr<Texture2D> get_white_texture();
};

} // namespace SE
