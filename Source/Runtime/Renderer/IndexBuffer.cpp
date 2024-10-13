/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Renderer/IndexBuffer.h>
#include <Renderer/RendererAPI.h>

#if SE_RENDERER_API_SUPPORTED_D3D11
    #include <Renderer/Platform/D3D11/D3D11IndexBuffer.h>
#endif // SE_RENDERER_API_SUPPORTED_D3D11

namespace SE
{

RefPtr<IndexBuffer> IndexBuffer::create(const IndexBufferDescription& description)
{
    switch (get_current_renderer_api())
    {
#if SE_RENDERER_API_SUPPORTED_D3D11
        case RendererAPI::D3D11: return create_ref<D3D11IndexBuffer>(description).as<IndexBuffer>();
#endif // SE_RENDERER_API_SUPPORTED_D3D11
    }

    SE_ASSERT(false);
    return {};
}

} // namespace SE
