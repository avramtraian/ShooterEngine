/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Framebuffer.h"
#include "Renderer/Platform/D3D11/D3D11Headers.h"

namespace SE
{

class D3D11Framebuffer final : public Framebuffer
{
public:
    struct Attachment
    {
        FramebufferAttachment   info;
        DXGI_FORMAT             format;
        ID3D11RenderTargetView* view;
        ID3D11Texture2D* image;
    };

public:
    D3D11Framebuffer(const FramebufferInfo& info);
    virtual ~D3D11Framebuffer() override;

    const Vector<Attachment>& get_attachments() const { return m_attachments; }
    ALWAYS_INLINE u32 get_width() const { return m_width; }
    ALWAYS_INLINE u32 get_height() const { return m_height; }

private:
    bool m_is_swapchain_target;
    Vector<Attachment> m_attachments;
    u32 m_width;
    u32 m_height;
};

} // namespace SE