/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreTypes.h"

#if SE_PLATFORM_WINDOWS
    #define SE_RENDERER_API_SUPPORTED_D3D11  1
    #define SE_RENDERER_API_SUPPORTED_D3D12  1
    #define SE_RENDERER_API_SUPPORTED_VULKAN 1
#endif // Platform switch.

namespace SE
{

enum class RendererAPI : u8
{
    None,
    D3D11,
    D3D12,
    Vulkan,
};

SHOOTER_API RendererAPI get_current_renderer_api();
SHOOTER_API void set_current_renderer_api(RendererAPI renderer_api);

SHOOTER_API bool is_renderer_api_available_on_current_platform();
SHOOTER_API RendererAPI get_recommended_renderer_api_for_current_platform();

} // namespace SE