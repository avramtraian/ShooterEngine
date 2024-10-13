/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Renderer/RendererAPI.h>
#include <Renderer/Texture.h>

#if SE_RENDERER_API_SUPPORTED_D3D11
    #include "Renderer/Platform/D3D11/D3D11Texture.h"
#endif // SE_RENDERER_API_SUPPORTED_D3D11

namespace SE
{

RefPtr<Texture2D> Texture2D::create(const Texture2DDescription& description)
{
    switch (get_current_renderer_api())
    {
#if SE_RENDERER_API_SUPPORTED_D3D11
        case RendererAPI::D3D11: return create_ref<D3D11Texture2D>(description).as<Texture2D>();
#endif // SE_RENDERER_API_SUPPORTED_D3D11
    }

    SE_ASSERT(false);
    return {};
}

} // namespace SE
