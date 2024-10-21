/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Application/Window.h>
#include <Renderer/Platform/D3D11/D3D11Framebuffer.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>
#include <Renderer/Platform/D3D11/D3D11RenderingContext.h>

namespace SE
{

D3D11RenderingContext::D3D11RenderingContext(Window* window_context)
    : m_window_context(window_context)
{
    // Set the swapchain immutable configuration parameters.
    m_swapchain.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    m_swapchain.image_count = 2;

    // Create the swapchain.
    invalidate(window_context->get_client_area_width(), window_context->get_client_area_height());
}

D3D11RenderingContext::~D3D11RenderingContext()
{
    // NOTE: If there are still framebuffers alive it means the rendering context shouldn't be deleted, thus
    // is represents an invalid state. Continuing the execution will cause crashes in unexpected places.
    SE_VERIFY(m_swapchain.referenced_framebuffers.is_empty());
    destroy_swapchain();
}

void D3D11RenderingContext::invalidate(u32 new_width, u32 new_height)
{
    destroy_swapchain();

    m_swapchain.width = new_width;
    m_swapchain.height = new_height;

    //
    // The specification of the window associated swapchain.
    // https://learn.microsoft.com/en-us/windows/win32/api/dxgi1_2/ns-dxgi1_2-dxgi_swap_chain_desc1
    //
    DXGI_SWAP_CHAIN_DESC1 swapchain_description = {};
    swapchain_description.Width = static_cast<UINT>(m_swapchain.width);
    swapchain_description.Height = static_cast<UINT>(m_swapchain.height);
    swapchain_description.Format = m_swapchain.format;
    swapchain_description.SampleDesc.Count = 1;
    swapchain_description.SampleDesc.Quality = 0;
    swapchain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_description.BufferCount = m_swapchain.image_count;
    swapchain_description.Scaling = DXGI_SCALING_STRETCH;
    swapchain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // https://learn.microsoft.com/en-us/windows/win32/api/dxgi1_2/ns-dxgi1_2-dxgi_swap_chain_fullscreen_desc
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapchain_fullscreen_description = {};
    swapchain_fullscreen_description.Scaling = DXGI_MODE_SCALING_CENTERED;
    swapchain_fullscreen_description.Windowed = true;

    const HRESULT swapchain_creation_result = D3D11Renderer::get_dxgi_factory()->CreateSwapChainForHwnd(
        D3D11Renderer::get_device(),
        static_cast<HWND>(m_window_context->get_native_handle()),
        &swapchain_description,
        &swapchain_fullscreen_description,
        nullptr,
        &m_swapchain.handle
    );

    // NOTE: There is no point in trying to continue running the engine if the swapchain creation failed, as there will
    // be no context where the graphics will be rendered to. In the end, the program will exit (or crash) anyway.
    SE_VERIFY(SUCCEEDED(swapchain_creation_result));

    const HRESULT swapchain_acquire_image_result = m_swapchain.handle->GetBuffer(0, IID_PPV_ARGS(&m_swapchain.image_handle));
    SE_VERIFY(SUCCEEDED(swapchain_acquire_image_result));

    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_render_target_view_desc
    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_description = {};
    render_target_view_description.Format = m_swapchain.format;
    render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    const HRESULT swapchain_image_create_rtv_result =
        D3D11Renderer::get_device()->CreateRenderTargetView(m_swapchain.image_handle, &render_target_view_description, &m_swapchain.image_view_handle);
    SE_VERIFY(SUCCEEDED(swapchain_image_create_rtv_result));

    for (Framebuffer* referenced_framebuffer : m_swapchain.referenced_framebuffers)
    {
        // Invalidate all referenced framebuffers.
        referenced_framebuffer->invalidate(0, 0);
    }
}

void D3D11RenderingContext::destroy_swapchain()
{
    // Destroy all framebuffers that are referenced by the swapchain.
    for (D3D11Framebuffer* referenced_framebuffer : m_swapchain.referenced_framebuffers)
        referenced_framebuffer->destroy();

    SE_D3D11_RELEASE(m_swapchain.image_view_handle)
    SE_D3D11_RELEASE(m_swapchain.image_handle)
    SE_D3D11_RELEASE(m_swapchain.handle);

    m_swapchain.width = 0;
    m_swapchain.height = 0;
}

ImageFormat D3D11RenderingContext::get_swapchain_image_format() const
{
    switch (m_swapchain.format)
    {
        case DXGI_FORMAT_UNKNOWN: return ImageFormat ::Unknown;
        case DXGI_FORMAT_B8G8R8A8_UNORM: return ImageFormat::BGRA8;
    }

    SE_ASSERT(false);
    return ImageFormat::Unknown;
}

void* D3D11RenderingContext::get_swapchain_image(u32 image_index /*= 0*/) const
{
    SE_ASSERT(image_index == 0);
    return m_swapchain.image_handle;
}

void* D3D11RenderingContext::get_swapchain_image_view(u32 image_index /*= 0*/) const
{
    SE_ASSERT(image_index == 0);
    return m_swapchain.image_view_handle;
}

void D3D11RenderingContext::reference_swapchain_target_framebuffer(D3D11Framebuffer* framebuffer)
{
    for (const D3D11Framebuffer* referenced_framebuffer : m_swapchain.referenced_framebuffers)
    {
        // The provided framebuffer is already referenced.
        if (referenced_framebuffer == framebuffer)
            return;
    }

    // Add the framebuffer to the referenced list.
    m_swapchain.referenced_framebuffers.add(framebuffer);
}

void D3D11RenderingContext::dereference_swapchain_target_framebuffer(D3D11Framebuffer* framebuffer)
{
    Optional<usize> framebuffer_index;

    for (usize index = 0; index < m_swapchain.referenced_framebuffers.count(); ++index)
    {
        // The provided framebuffer is already referenced.
        if (m_swapchain.referenced_framebuffers[index] == framebuffer)
        {
            framebuffer_index = index;
            break;
        }
    }

    if (!framebuffer_index.has_value())
    {
        // The provided framebuffer is not referenced. Maybe this should raise an error?
        return;
    }

    // Remove the framebuffer from the referenced list.
    m_swapchain.referenced_framebuffers.remove_unordered(framebuffer_index.value());
}

} // namespace SE
