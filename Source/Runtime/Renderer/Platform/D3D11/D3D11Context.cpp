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

    DXGI_SWAP_CHAIN_DESC swapchain_descriptor = {};
    swapchain_descriptor.BufferDesc.Width = m_window->get_width();
    swapchain_descriptor.BufferDesc.Height = m_window->get_height();
    swapchain_descriptor.BufferDesc.RefreshRate.Numerator = 144;
    swapchain_descriptor.BufferDesc.RefreshRate.Denominator = 1;
    swapchain_descriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    swapchain_descriptor.SampleDesc.Count = 1;
    swapchain_descriptor.SampleDesc.Quality = 0;
    swapchain_descriptor.BufferUsage = DXGI_USAGE_BACK_BUFFER;
    swapchain_descriptor.BufferCount = (UINT)(m_back_buffer_count);
    swapchain_descriptor.OutputWindow = (HWND)(info.window->get_native_handle());
    swapchain_descriptor.Windowed = true;
    swapchain_descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    SE_D3D11_CHECK(D3D11Renderer::get_dxgi_factory()->CreateSwapChain(D3D11Renderer::get_device(), &swapchain_descriptor, &m_swapchain));
}

D3D11Context::~D3D11Context()
{
    m_swapchain->Release();
    m_swapchain = nullptr;
}

} // namespace SE