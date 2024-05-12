/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/RefPtr.h"
#include "Core/Containers/Vector.h"
#include "Renderer/Image.h"

namespace SE
{

struct FramebufferAttachment
{
    ImageFormat      format         = ImageFormat::Unknown;
    ImageFiltering   min_filtering  = ImageFiltering::Linear;
    ImageFiltering   mag_filtering  = ImageFiltering::Linear;
    ImageAddressMode address_mode_u = ImageAddressMode::ClampToEdge;
    ImageAddressMode address_mode_v = ImageAddressMode::ClampToEdge;
};

struct FramebufferInfo
{
    // When this flag is set to true, all other parameters are ignored.
    // The resulting framebuffer will contain the back buffers of the swapchain.
    bool                          is_swapchain_target = false;
    // The context that owns the swapchain.
    // Only used when 'is_swapchain_target' is true.
    class RenderingContext*       rendering_context = nullptr;

    u32                           width;
    u32                           height;
    Vector<FramebufferAttachment> attachments;
};

class Framebuffer : public RefCounted
{
public:
    Framebuffer() = default;
    virtual ~Framebuffer() override = default;

    static RefPtr<Framebuffer> create(const FramebufferInfo& info);

public:
};

} // namespace SE