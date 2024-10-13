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

class D3D11RenderingContext final : public RenderingContext
{
public:
    explicit D3D11RenderingContext(Window* window_context);
    virtual ~D3D11RenderingContext() override;

    virtual void invalidate(u32 new_width, u32 new_height) override;

    NODISCARD ALWAYS_INLINE virtual u32 get_swapchain_width() const override { return m_swapchain.width; }
    NODISCARD ALWAYS_INLINE virtual u32 get_swapchain_height() const override { return m_swapchain.height; }
    NODISCARD ALWAYS_INLINE virtual ImageFormat get_swapchain_image_format() const override;

    NODISCARD virtual void* get_swapchain_image(u32 image_index = 0) const override;
    NODISCARD virtual void* get_swapchain_image_view(u32 image_index = 0) const override;
    NODISCARD ALWAYS_INLINE IDXGISwapChain1* get_swapchain() const { return m_swapchain.handle; }

    void reference_swapchain_target_framebuffer(D3D11Framebuffer* framebuffer);
    void dereference_swapchain_target_framebuffer(D3D11Framebuffer* framebuffer);

private:
    void destroy_swapchain();

private:
    struct Swapchain
    {
        IDXGISwapChain1* handle { nullptr };
        ID3D11Texture2D* image_handle { nullptr };
        ID3D11RenderTargetView* image_view_handle { nullptr };

        u32 width { 0 };
        u32 height { 0 };
        DXGI_FORMAT format { DXGI_FORMAT_UNKNOWN };
        u32 image_count { 0 };

        Vector<D3D11Framebuffer*> referenced_framebuffers;
    };

    Window* m_window_context;
    Swapchain m_swapchain;
};

} // namespace SE
