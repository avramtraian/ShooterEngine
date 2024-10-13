/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Renderer/RendererAPI.h>
#include <Renderer/RendererInterface.h>

#if SE_RENDERER_API_SUPPORTED_D3D11
    #include "Renderer/Platform/D3D11/D3D11Renderer.h"
#endif // SE_RENDERER_API_SUPPORTED_D3D11

namespace SE
{

OwnPtr<RendererInterface> RendererInterface::create()
{
    switch (get_current_renderer_api())
    {
#if SE_RENDERER_API_SUPPORTED_D3D11
        case RendererAPI::D3D11: return create_own<D3D11Renderer>().as<RendererInterface>();
#endif // SE_RENDERER_API_SUPPORTED_D3D11
    }

    SE_ASSERT(false);
    return {};
}

} // namespace SE
