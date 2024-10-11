/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Renderer/Platform/D3D11/D3D11Context.h>
#include <Renderer/Platform/D3D11/D3D11Framebuffer.h>
#include <Renderer/Platform/D3D11/D3D11Image.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>

namespace SE
{

D3D11Framebuffer::D3D11Framebuffer(const FramebufferInfo& info)
    : m_width(0)
    , m_height(0)
    , m_is_swapchain_target(info.is_swapchain_target)
    , m_swapchain_target_context(static_cast<D3D11Context*>(info.rendering_context))
{
    u32 width = 0;
    u32 height = 0;

    if (m_is_swapchain_target)
    {
        SE_ASSERT(m_swapchain_target_context != nullptr);

        Attachment attachment = {};
        attachment.info = {};
        switch (m_swapchain_target_context->get_swapchain_format())
        {
            case DXGI_FORMAT_B8G8R8A8_UNORM: attachment.info.format = ImageFormat::BGRA8; break;
            default: SE_ASSERT(false); return; // Unknown swapchain format.
        }

        attachment.format = m_swapchain_target_context->get_swapchain_format();
        m_attachments.add(attachment);

        width = m_swapchain_target_context->get_swapchain_width();
        height = m_swapchain_target_context->get_swapchain_height();
    }
    else
    {
        for (usize attachment_index = 0; attachment_index < info.attachments.count(); ++attachment_index)
        {
            Attachment attachment = {};
            attachment.info = info.attachments[attachment_index];
            attachment.format = d3d11_image_format(attachment.info.format);
            m_attachments.add(move(attachment));
        }

        width = info.width;
        height = info.height;
    }

    invalidate(width, height);
}

D3D11Framebuffer::~D3D11Framebuffer()
{
    destroy();
}

void D3D11Framebuffer::resize(u32 new_width, u32 new_height)
{
    if (m_width == new_width && m_height == new_height)
        return;

    // Create a new D3D11 framebuffer object (and destroy the current one).
    invalidate(new_width, new_height);
}

void D3D11Framebuffer::invalidate(u32 width, u32 height)
{
    destroy();
    m_width = width;
    m_height = height;

    if (m_is_swapchain_target)
    {
        SE_ASSERT(m_attachments.count() == 1);
        Attachment& attachment = m_attachments.first();

        // Ensure that the swapchain was resized prior to resizing the framebuffer object.
        SE_ASSERT(m_width == m_swapchain_target_context->get_swapchain_width() && m_height == m_swapchain_target_context->get_swapchain_height());

        SE_ASSERT(m_swapchain_target_context != nullptr);
        m_swapchain_target_context->add_framebuffer_reference(this);

        attachment.image = m_swapchain_target_context->get_swapchain_image();
        attachment.view = m_swapchain_target_context->get_swapchain_render_target_view();
    }
    else
    {
        for (Attachment& attachment : m_attachments)
        {
            D3D11_TEXTURE2D_DESC texture_description = {};
            texture_description.Width = (UINT)(m_width);
            texture_description.Height = (UINT)(m_height);
            texture_description.MipLevels = 1;
            texture_description.ArraySize = 1;
            texture_description.Format = attachment.format;
            texture_description.SampleDesc.Count = 1;
            texture_description.SampleDesc.Quality = 0;
            texture_description.Usage = D3D11_USAGE_DEFAULT;
            texture_description.BindFlags = D3D11_BIND_RENDER_TARGET;
            texture_description.CPUAccessFlags = 0;

            D3D11_RENDER_TARGET_VIEW_DESC render_target_view_description = {};
            render_target_view_description.Format = attachment.format;
            render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            render_target_view_description.Texture2D.MipSlice = 0;

            SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateTexture2D(&texture_description, nullptr, &attachment.image));
            SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateRenderTargetView(attachment.image, &render_target_view_description, &attachment.view));
        }
    }
}

void D3D11Framebuffer::destroy()
{
    if (m_is_swapchain_target)
    {
        SE_ASSERT(m_attachments.count() == 1);
        if (m_attachments.first().image)
        {
            SE_ASSERT(m_swapchain_target_context != nullptr);
            m_swapchain_target_context->remove_framebuffer_reference(this);
        }
    }

    for (Attachment& attachment : m_attachments)
    {
        if (attachment.view)
            attachment.view = nullptr;

        if (attachment.image)
            attachment.image = nullptr;
    }

    m_width = 0;
    m_height = 0;
}

} // namespace SE
