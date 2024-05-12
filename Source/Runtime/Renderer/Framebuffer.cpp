/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Renderer/Framebuffer.h"
#include "Renderer/RendererAPI.h"

#if SE_RENDERER_API_SUPPORTED_D3D11
    #include "Renderer/Platform/D3D11/D3D11Framebuffer.h"
#endif // SE_RENDERER_API_SUPPORTED_D3D11

namespace SE
{

RefPtr<Framebuffer> Framebuffer::create(const FramebufferInfo& info)
{
	switch (get_current_renderer_api())
	{
#if SE_RENDERER_API_SUPPORTED_D3D11
		case RendererAPI::D3D11:
			return make_ref<D3D11Framebuffer>(info).as<Framebuffer>();
#endif // SE_RENDERER_API_SUPPORTED_D3D11
	}

	SE_ASSERT(false);
	return {};
}

} // namespace SE