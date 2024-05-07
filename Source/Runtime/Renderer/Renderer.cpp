/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/Containers/HashMap.h"
#include "Core/Containers/OwnPtr.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/RendererInterface.h"

namespace SE
{

struct RendererData
{
    OwnPtr<RendererInterface> interface;
    HashMap<void*, OwnPtr<RenderingContext>> context_table;
    RenderingContext* active_rendering_context = nullptr;
};

static OwnPtr<RendererData> s_renderer;

bool Renderer::initialize()
{
    if (s_renderer.is_valid())
        return false;
    s_renderer = make_own<RendererData>();
    
    RendererAPI renderer_api = get_recommended_renderer_api_for_current_platform();
    set_current_renderer_api(renderer_api);

    s_renderer->interface = RendererInterface::create();
    if (!s_renderer->interface->initialize())
    {
        SE_LOG_ERROR("Failed to create the renderer interface!"sv);
        return false;
    }

    return true;
}

void Renderer::shutdown()
{
    if (!s_renderer.is_valid())
        return;

    s_renderer->context_table.clear();

    s_renderer->interface->shutdown();
    s_renderer->interface.release();

    s_renderer.release();
}


bool Renderer::create_context_for_window(Window* window)
{
    RenderingContextInfo info = {};
    info.window = window;
    
    OwnPtr<RenderingContext>& context = s_renderer->context_table.get_or_add(window->get_native_handle());
    context = RenderingContext::create(info);
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

} // namespace SE