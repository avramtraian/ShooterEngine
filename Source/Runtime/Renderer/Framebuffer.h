/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Badge.h>
#include <Core/Containers/RefPtr.h>
#include <Core/Containers/Vector.h>
#include <Renderer/Image.h>

namespace SE
{

// Forward declarations.
class RenderingContext;

struct FramebufferAttachmentDescription
{
    ALWAYS_INLINE FramebufferAttachmentDescription()
        : format(ImageFormat::BGRA8)
    {}

    ALWAYS_INLINE FramebufferAttachmentDescription(ImageFormat in_format)
        : format(in_format)
    {}

    ImageFormat format { ImageFormat::Unknown };
    bool use_as_input_texture { false };

    ImageFilteringMode min_filtering_mode { ImageFilteringMode::Linear };
    ImageFilteringMode mag_filtering_mode { ImageFilteringMode::Linear };

    ImageAddressMode address_mode_u { ImageAddressMode::Repeat };
    ImageAddressMode address_mode_v { ImageAddressMode::Repeat };
    ImageAddressMode address_mode_w { ImageAddressMode::Repeat };
};

struct FramebufferDescription
{
    u32 width { 0 };
    u32 height { 0 };
    Vector<FramebufferAttachmentDescription> attachments;
};

class Framebuffer : public RefCounted
{
public:
    //
    // Creates a new framebuffer by allocating a new image for each attachment.
    //
    NODISCARD SHOOTER_API static RefPtr<Framebuffer> create(const FramebufferDescription& description);

    //
    // Creates a new framebuffer that represents a swapchain target, meaning that the attachment images are not allocated
    // from scratch and instead they reference an image of the swapchain.
    //
    NODISCARD SHOOTER_API static RefPtr<Framebuffer> create(Badge<class Renderer>, RenderingContext& context);

    Framebuffer() = default;
    virtual ~Framebuffer() override = default;

public:
    //
    // Invalidates the current framebuffer by resizing its attachments.
    // If the framebuffer is a swapchain target, the provided width and height parameters are ignored (and
    // thus should be set to zero) and the dimensions of the framebuffer will be determined by the swapchain.
    //
    virtual void invalidate(u32 new_width, u32 new_height) = 0;

    NODISCARD virtual bool is_swapchain_target() const = 0;

    NODISCARD virtual u32 get_width() const = 0;
    NODISCARD virtual u32 get_height() const = 0;
    NODISCARD virtual u32 get_attachment_count() const = 0;

    NODISCARD virtual void* get_attachment_image(u32 attachment_index) const = 0;
    // NOTE: Returns a null pointer if the specified framebuffer attachment hasn't been created with the `use_as_input_texture` flag.
    NODISCARD virtual void* get_attachment_image_view(u32 attachment_index) const = 0;
    NODISCARD virtual const FramebufferAttachmentDescription& get_attachment_description(u32 attachment_index) const = 0;
};

} // namespace SE
