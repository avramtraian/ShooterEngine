/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Renderer/RendererAPI.h>

namespace SE
{

static RendererAPI s_current_renderer_api = RendererAPI::None;

RendererAPI get_current_renderer_api()
{
    return s_current_renderer_api;
}

void set_current_renderer_api(RendererAPI renderer_api)
{
    s_current_renderer_api = renderer_api;
}

bool is_renderer_api_available_on_current_platform()
{
#if SE_PLATFORM_WINDOWS
    // All renderer APIs are supported on Windows.
    return true;
#endif // SE_PLATFORM_WINDOWS
}

RendererAPI get_recommended_renderer_api_for_current_platform()
{
#if SE_PLATFORM_WINDOWS
    return RendererAPI::D3D11;
#endif // SE_PLATFORM_WINDOWS
}

} // namespace SE
