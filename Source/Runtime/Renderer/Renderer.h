/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Engine/Application/Window.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/RenderingContext.h"
#include "Renderer/RenderPass.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexBuffer.h"

namespace SE
{

class Renderer
{
public:
    SHOOTER_API static bool initialize();
    SHOOTER_API static void shutdown();
    SHOOTER_API static bool is_initialized();

    SHOOTER_API static bool create_context_for_window(Window* window);
    SHOOTER_API static RenderingContext* get_rendering_context_for_window(Window* window);
    
    SHOOTER_API static RenderingContext* get_active_context();
    SHOOTER_API static void set_active_context(RenderingContext* context);

public:
    SHOOTER_API static void begin_frame();
    SHOOTER_API static void end_frame();

    SHOOTER_API static void on_resize(u32 new_width, u32 new_height);

    static void begin_render_pass(RefPtr<RenderPass> render_pass);
    static void end_render_pass();

    static void draw_indexed(RefPtr<VertexBuffer> vertex_buffer, RefPtr<IndexBuffer> index_buffer, u32 index_count);

public:
    static RefPtr<Texture2D> get_white_texture();
    static RefPtr<Texture2D> get_black_texture();
};

} // namespace SE