/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Renderer/Framebuffer.h>
#include <Renderer/Platform/D3D11/D3D11Headers.h>

namespace SE
{

class D3D11Framebuffer final : public Framebuffer
{
public:
    struct Attachment
    {
        FramebufferAttachment info;
        DXGI_FORMAT format;
        ID3D11RenderTargetView* view;
        ID3D11Texture2D* image;
    };

public:
    D3D11Framebuffer(const FramebufferInfo& info);
    virtual ~D3D11Framebuffer() override;

    virtual void resize(u32 new_width, u32 new_height) override;

    NODISCARD ALWAYS_INLINE const Vector<Attachment>& get_attachments() const { return m_attachments; }
    NODISCARD ALWAYS_INLINE u32 get_width() const { return m_width; }
    NODISCARD ALWAYS_INLINE u32 get_height() const { return m_height; }

public:
    void invalidate(u32 width, u32 height);
    void destroy();

private:
    Vector<Attachment> m_attachments;
    u32 m_width;
    u32 m_height;

    bool m_is_swapchain_target;
    // Only used when the framebuffer is a swapchain target.
    class D3D11Context* m_swapchain_target_context;
};

} // namespace SE
