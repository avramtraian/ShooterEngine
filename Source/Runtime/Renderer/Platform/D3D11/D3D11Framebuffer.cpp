/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Renderer/Platform/D3D11/D3D11Framebuffer.h"
#include "Renderer/Platform/D3D11/D3D11Image.h"
#include "Renderer/Platform/D3D11/D3D11Renderer.h"

namespace SE
{

D3D11Framebuffer::D3D11Framebuffer(const FramebufferInfo& info)
    : m_is_swapchain_target(info.is_swapchain_target)
    , m_width(info.width)
    , m_height(info.height)
{
    if (m_is_swapchain_target)
    {
        SE_ASSERT(info.rendering_context != nullptr);
        D3D11Context* context = static_cast<D3D11Context*>(info.rendering_context);

        m_width = context->get_swapchain_width();
        m_height = context->get_swapchain_height();

        Attachment attachment = {};
        attachment.info = {};
        // Use the default attachment info settings, except for the format.
        switch (context->get_swapchain_format())
        {
            case DXGI_FORMAT_B8G8R8A8_UNORM: attachment.info.format = ImageFormat::BGRA8; break;

            default: SE_ASSERT(false); return; // Unknown swapchain format.
        }

        attachment.format = context->get_swapchain_format();
        attachment.image = context->get_swapchain_image();
        attachment.view = context->get_swapchain_render_target_view();
        m_attachments.add(attachment);

        attachment.image->AddRef();
        attachment.view->AddRef();

        return;
    }

    for (usize attachment_index = 0; attachment_index < info.attachments.count(); ++attachment_index)
    {
        Attachment attachment = {};
        attachment.info = info.attachments[attachment_index];
        attachment.format = d3d11_image_format(attachment.info.format);

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

        m_attachments.add(attachment);
    }
}

D3D11Framebuffer::~D3D11Framebuffer()
{
    for (const Attachment& attachment : m_attachments)
    {
        if (attachment.view)
            attachment.view->Release();
        if (attachment.image)
            attachment.image->Release();
    }

    m_attachments.clear_and_shrink();
}

} // namespace SE