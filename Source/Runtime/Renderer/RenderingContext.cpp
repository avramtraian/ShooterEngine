/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Renderer/RendererAPI.h>
#include <Renderer/RenderingContext.h>

#if SE_RENDERER_API_SUPPORTED_D3D11
    #include "Renderer/Platform/D3D11/D3D11Context.h"
#endif // SE_RENDERER_API_SUPPORTED_D3D11

namespace SE
{

OwnPtr<RenderingContext> RenderingContext::create(const RenderingContextInfo& info)
{
    switch (get_current_renderer_api())
    {
#if SE_RENDERER_API_SUPPORTED_D3D11
        case RendererAPI::D3D11: return make_own<D3D11Context>(info).as<RenderingContext>();
#endif // SE_RENDERER_API_SUPPORTED_D3D11
    }

    SE_ASSERT(false);
    return {};
}

} // namespace SE
