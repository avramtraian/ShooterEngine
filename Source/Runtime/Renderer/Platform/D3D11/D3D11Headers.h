/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Platform/Windows/WindowsHeaders.h>
#include <Renderer/RendererAPI.h>

#if SE_RENDERER_API_SUPPORTED_D3D11
    #include <d3d11.h>
    #include <dxgi1_3.h>
#endif // SE_RENDERER_API_SUPPORTED_D3D11
