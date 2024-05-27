/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Renderer/Renderer.h"
#include "Renderer/WorldRenderer.h"

namespace SE
{

bool WorldRenderer::initialize(u32 width, u32 height)
{
    SE_ASSERT(!m_renderer_2d.is_valid());
    m_renderer_2d = make_own<Renderer2D>();
    if (!m_renderer_2d->initialize(width, height))
    {
        SE_LOG_TAG_ERROR("Renderer"sv, "Failed to initialize the 2D renderer!"sv);
        return false;
    }

    return true;
}

void WorldRenderer::shutdown()
{
    m_renderer_2d->shutdown();
    m_renderer_2d.release();
}

bool WorldRenderer::render()
{
    Renderer::begin_frame();
    m_renderer_2d->begin_frame();

    m_renderer_2d->submit_quad(-0.6666F, 0.0F, 0.6666F, 1.0F, Color3(0.1F, 0.1F, 0.8F));
    m_renderer_2d->submit_quad(+0.0F,    0.0F, 0.6666F, 1.0F, Color3(0.8F, 0.8F, 0.1F));
    m_renderer_2d->submit_quad(+0.6666F, 0.0F, 0.6666F, 1.0F, Color3(0.8F, 0.1F, 0.1F));

    m_renderer_2d->end_frame();
    Renderer::end_frame();
    return true;
}

void WorldRenderer::on_resize(u32 new_width, u32 new_height)
{
    // Resize the 2D renderer.
    m_renderer_2d->resize_target_framebuffer(new_width, new_height);
}

} // namespace SE
