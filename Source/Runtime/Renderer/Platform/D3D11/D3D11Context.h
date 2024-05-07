/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/RenderingContext.h"

#include <d3d11.h>

namespace SE
{

class D3D11Context final : public RenderingContext
{
public:
    D3D11Context(const RenderingContextInfo& info);
    virtual ~D3D11Context() override;

private:
    IDXGISwapChain* m_swapchain;
    u32 m_back_buffer_count;
    class Window* m_window;
};

} // namespace SE