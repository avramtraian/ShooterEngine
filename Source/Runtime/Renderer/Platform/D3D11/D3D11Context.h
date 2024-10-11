/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Renderer/Platform/D3D11/D3D11Framebuffer.h>
#include <Renderer/Platform/D3D11/D3D11Headers.h>
#include <Renderer/RenderingContext.h>

namespace SE
{

class D3D11Context final : public RenderingContext
{
public:
    D3D11Context(const RenderingContextInfo& info);
    virtual ~D3D11Context() override;

    virtual void resize(u32 new_width, u32 new_height) override;

    ALWAYS_INLINE IDXGISwapChain1* get_swapchain() const { return m_swapchain; }
    ALWAYS_INLINE DXGI_FORMAT get_swapchain_format() const { return m_swapchain_format; }
    ALWAYS_INLINE u32 get_swapchain_width() const { return m_swapchain_width; }
    ALWAYS_INLINE u32 get_swapchain_height() const { return m_swapchain_height; }

    ALWAYS_INLINE ID3D11Texture2D* get_swapchain_image() const { return m_swapchain_image; }
    ALWAYS_INLINE ID3D11RenderTargetView* get_swapchain_render_target_view() const { return m_swapchain_render_target_view; }

    void add_framebuffer_reference(D3D11Framebuffer* framebuffer);
    void remove_framebuffer_reference(D3D11Framebuffer* framebuffer);

private:
    void invalidate();
    void destroy();

private:
    class Window* m_window;

    IDXGISwapChain1* m_swapchain;
    DXGI_FORMAT m_swapchain_format;
    u32 m_swapchain_width;
    u32 m_swapchain_height;
    u32 m_swapchain_image_count;

    ID3D11Texture2D* m_swapchain_image;
    ID3D11RenderTargetView* m_swapchain_render_target_view;

    // List of all framebuffer that target a swapchain image.
    Vector<D3D11Framebuffer*> m_associated_framebuffers;
};

} // namespace SE
