/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Engine/Application/Window.h"
#include "Renderer/Platform/D3D11/D3D11Context.h"
#include "Renderer/Platform/D3D11/D3D11Renderer.h"

namespace SE
{

D3D11Context::D3D11Context(const RenderingContextInfo& info)
    : m_swapchain(nullptr)
    , m_back_buffer_count(0)
    , m_window(info.window)
{
    m_back_buffer_count = 2;

    DXGI_SWAP_CHAIN_DESC1 swapchain_description = {};
    swapchain_description.Width = m_window->get_width();
    swapchain_description.Height = m_window->get_height();
    swapchain_description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapchain_description.SampleDesc.Count = 1;
    swapchain_description.SampleDesc.Quality = 0;
    swapchain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_description.BufferCount = (UINT)(m_back_buffer_count);
    swapchain_description.Scaling = DXGI_SCALING_STRETCH;
    swapchain_description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapchain_fullscreen_description = {};
    swapchain_fullscreen_description.Scaling = DXGI_MODE_SCALING_CENTERED;
    swapchain_fullscreen_description.Windowed = true;

    SE_D3D11_CHECK(D3D11Renderer::get_dxgi_factory()->CreateSwapChainForHwnd(
        D3D11Renderer::get_device(), (HWND)(info.window->get_native_handle()),
        &swapchain_description, &swapchain_fullscreen_description, nullptr, &m_swapchain
    ));
}

D3D11Context::~D3D11Context()
{
    m_swapchain->Release();
    m_swapchain = nullptr;
}

} // namespace SE