/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Renderer/Platform/D3D11/D3D11Framebuffer.h>
#include <Renderer/Platform/D3D11/D3D11Image.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>
#include <Renderer/Platform/D3D11/D3D11RenderingContext.h>

namespace SE
{

D3D11Framebuffer::D3D11Framebuffer(const FramebufferDescription& description)
    : m_is_swapchain_target(false)
    , m_width(0)
    , m_height(0)
{
    m_attachments.set_fixed_capacity(description.attachments.count());
    for (const auto& attachment_description : description.attachments)
    {
        Attachment attachment = {};
        attachment.description = attachment_description;
        m_attachments.add(attachment);
    }

    invalidate(description.width, description.height);
}

D3D11Framebuffer::D3D11Framebuffer(RenderingContext& rendering_context)
    : m_is_swapchain_target(true)
    , m_rendering_context(static_cast<D3D11RenderingContext&>(rendering_context))
    , m_width(0)
    , m_height(0)
{
    m_rendering_context->reference_swapchain_target_framebuffer(this);

    Attachment attachment = {};
    attachment.description.format = rendering_context.get_swapchain_image_format();
    m_attachments.add(move(attachment));

    invalidate(0, 0);
}

D3D11Framebuffer::~D3D11Framebuffer()
{
    if (m_is_swapchain_target)
    {
        SE_ASSERT(m_rendering_context.has_value());
        m_rendering_context->dereference_swapchain_target_framebuffer(this);
    }

    destroy();
}

void D3D11Framebuffer::invalidate(u32 new_width, u32 new_height)
{
    if (m_is_swapchain_target)
    {
        // Forward the invalidation implementation to the swapchain target version.
        invalidate_swapchain_target();
        return;
    }

    destroy();
    m_width = new_width;
    m_height = new_height;

    for (Attachment& attachment : m_attachments)
    {
        //
        // The specification of the framebuffer attachment image (texture).
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_texture2d_desc
        //
        D3D11_TEXTURE2D_DESC texture_description = {};
        texture_description.Width = static_cast<UINT>(m_width);
        texture_description.Height = static_cast<UINT>(m_height);
        texture_description.MipLevels = 1;
        texture_description.ArraySize = 1;
        texture_description.Format = get_d3d11_image_format(attachment.description.format);
        texture_description.SampleDesc.Count = 1;
        texture_description.SampleDesc.Quality = 0;
        texture_description.Usage = D3D11_USAGE_DEFAULT;
        texture_description.BindFlags = D3D11_BIND_RENDER_TARGET;
        texture_description.CPUAccessFlags = 0;

        //
        // The specification of the framebuffer attachment image view (RTV).
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_render_target_view_desc
        //
        D3D11_RENDER_TARGET_VIEW_DESC render_target_view_description = {};
        render_target_view_description.Format = get_d3d11_image_format(attachment.description.format);
        render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        render_target_view_description.Texture2D.MipSlice = 0;

        const HRESULT image_creation_result = D3D11Renderer::get_device()->CreateTexture2D(&texture_description, nullptr, &attachment.image_handle);
        SE_ASSERT(SUCCEEDED(image_creation_result));

        const HRESULT image_rtv_creation_result =
            D3D11Renderer::get_device()->CreateRenderTargetView(attachment.image_handle, &render_target_view_description, &attachment.image_rtv_handle);
        SE_ASSERT(SUCCEEDED(image_rtv_creation_result));
    }
}

void D3D11Framebuffer::invalidate_swapchain_target()
{
    destroy();

    SE_ASSERT(m_is_swapchain_target && m_rendering_context.has_value());
    SE_ASSERT(m_attachments.count() == 1);

    Attachment& attachment = m_attachments.first();
    attachment.image_handle = static_cast<ID3D11Texture2D*>(m_rendering_context->get_swapchain_image());
    attachment.image_rtv_handle = static_cast<ID3D11RenderTargetView*>(m_rendering_context->get_swapchain_image_view());

    m_width = m_rendering_context->get_swapchain_width();
    m_height = m_rendering_context->get_swapchain_height();
}

void D3D11Framebuffer::destroy()
{
    for (Attachment& attachment : m_attachments)
    {
        if (m_is_swapchain_target)
        {
            attachment.image_handle = nullptr;
            attachment.image_rtv_handle = nullptr;
        }
        else
        {
            SE_D3D11_RELEASE(attachment.image_handle);
            SE_D3D11_RELEASE(attachment.image_rtv_handle);
        }
    }

    m_width = 0;
    m_height = 0;
}

void* D3D11Framebuffer::get_attachment_image(u32 attachment_index) const
{
    SE_ASSERT(attachment_index < m_attachments.count());
    return m_attachments[attachment_index].image_handle;
}

void* D3D11Framebuffer::get_attachment_image_view(u32 attachment_index) const
{
    SE_ASSERT(attachment_index < m_attachments.count());
    return m_attachments[attachment_index].image_rtv_handle;
}

const FramebufferAttachmentDescription& D3D11Framebuffer::get_attachment_description(u32 attachment_index) const
{
    SE_ASSERT(attachment_index < m_attachments.count());
    return m_attachments[attachment_index].description;
}

} // namespace SE
