/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/OwnPtr.h>
#include <Core/Containers/RefPtr.h>
#include <Renderer/IndexBuffer.h>
#include <Renderer/RenderPass.h>
#include <Renderer/RendererDevice.h>
#include <Renderer/VertexBuffer.h>

namespace SE
{

class RendererInterface
{
public:
    static OwnPtr<RendererInterface> create();

public:
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

public:
    virtual void on_resize(u32 new_width, u32 new_height) = 0;

    NODISCARD virtual RendererDevice get_renderer_device() const = 0;

    virtual void present(class RenderingContext* context) = 0;

    virtual void begin_render_pass(RefPtr<RenderPass> render_pass) = 0;
    virtual void end_render_pass() = 0;

    virtual void draw_indexed(RefPtr<VertexBuffer> vertex_buffer, RefPtr<IndexBuffer> index_buffer, u32 index_count) = 0;
};

} // namespace SE
