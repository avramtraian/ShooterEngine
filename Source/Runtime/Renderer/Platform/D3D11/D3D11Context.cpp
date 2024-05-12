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
    : m_window(info.window)
    , m_swapchain(nullptr)
    , m_swapchain_format(DXGI_FORMAT_UNKNOWN)
    , m_swapchain_width(0)
    , m_swapchain_height(0)
    , m_swapchain_image_count(0)
    , m_swapchain_image(nullptr)
    , m_swapchain_render_target_view(nullptr)
{
    m_swapchain_image_count = 2;
    m_swapchain_format = DXGI_FORMAT_B8G8R8A8_UNORM;
    m_swapchain_width = m_window->get_width();
    m_swapchain_height = m_window->get_height();

    DXGI_SWAP_CHAIN_DESC1 swapchain_description = {};
    swapchain_description.Width = (UINT)(m_swapchain_width);
    swapchain_description.Height = (UINT)(m_swapchain_height);
    swapchain_description.Format = m_swapchain_format;
    swapchain_description.SampleDesc.Count = 1;
    swapchain_description.SampleDesc.Quality = 0;
    swapchain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_description.BufferCount = (UINT)(m_swapchain_image_count);
    swapchain_description.Scaling = DXGI_SCALING_STRETCH;
    swapchain_description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapchain_fullscreen_description = {};
    swapchain_fullscreen_description.Scaling = DXGI_MODE_SCALING_CENTERED;
    swapchain_fullscreen_description.Windowed = true;

    SE_D3D11_CHECK(D3D11Renderer::get_dxgi_factory()->CreateSwapChainForHwnd(
        D3D11Renderer::get_device(), (HWND)(info.window->get_native_handle()),
        &swapchain_description, &swapchain_fullscreen_description, nullptr, &m_swapchain
    ));

    SE_D3D11_CHECK(m_swapchain->GetBuffer(0, IID_PPV_ARGS(&m_swapchain_image)));

    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_description = {};
    render_target_view_description.Format = m_swapchain_format;
    render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    render_target_view_description.Texture2D.MipSlice = 0;

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateRenderTargetView(m_swapchain_image, &render_target_view_description, &m_swapchain_render_target_view));
}

D3D11Context::~D3D11Context()
{
    if (m_swapchain_image)
    {
        m_swapchain_image->Release();
        m_swapchain_image = nullptr;
    }

    if (m_swapchain_render_target_view)
    {
        m_swapchain_render_target_view->Release();
        m_swapchain_render_target_view = nullptr;
    }

    if (m_swapchain)
    {
        m_swapchain->Release();
        m_swapchain = nullptr;
    }
}

} // namespace SE