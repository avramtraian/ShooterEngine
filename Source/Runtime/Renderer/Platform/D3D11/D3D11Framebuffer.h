/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Optional.h>
#include <Renderer/Framebuffer.h>
#include <Renderer/Platform/D3D11/D3D11Headers.h>

namespace SE
{

// Forward declarations.
class D3D11RenderingContext;

class D3D11Framebuffer : public Framebuffer
{
    friend class D3D11RenderingContext;

public:
    explicit D3D11Framebuffer(const FramebufferDescription& description);
    explicit D3D11Framebuffer(RenderingContext& rendering_context);

    virtual ~D3D11Framebuffer() override;

public:
    virtual void invalidate(u32 new_width, u32 new_height) override;

    NODISCARD ALWAYS_INLINE virtual bool is_swapchain_target() const override { return m_is_swapchain_target; }

    NODISCARD ALWAYS_INLINE virtual u32 get_width() const override { return m_width; }
    NODISCARD ALWAYS_INLINE virtual u32 get_height() const override { return m_height; }
    NODISCARD ALWAYS_INLINE virtual u32 get_attachment_count() const override { return static_cast<u32>(m_attachments.count()); }

    NODISCARD virtual void* get_attachment_image(u32 attachment_index) const override;
    NODISCARD virtual void* get_attachment_image_view(u32 attachment_index) const override;
    NODISCARD virtual const FramebufferAttachmentDescription& get_attachment_description(u32 attachment_index) const override;

private:
    void invalidate_swapchain_target();

    void destroy();

private:
    struct Attachment
    {
        ID3D11Texture2D* image_handle;
        ID3D11RenderTargetView* image_rtv_handle;
        FramebufferAttachmentDescription description;
    };

    bool m_is_swapchain_target;
    // Has value only when the framebuffer is a swapchain target.
    Optional<D3D11RenderingContext&> m_rendering_context;

    u32 m_width;
    u32 m_height;
    Vector<Attachment> m_attachments;
};

} // namespace SE
