/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/OwnPtr.h>
#include <Renderer/Image.h>

namespace SE
{

// Forward declarations.
class Window;

class RenderingContext
{
    SE_MAKE_NONCOPYABLE(RenderingContext);
    SE_MAKE_NONMOVABLE(RenderingContext);

public:
    NODISCARD static OwnPtr<RenderingContext> create(Window* window_context);

    RenderingContext() = default;
    virtual ~RenderingContext() = default;

public:
    virtual void invalidate(u32 new_width, u32 new_height) = 0;

    NODISCARD virtual u32 get_swapchain_width() const = 0;
    NODISCARD virtual u32 get_swapchain_height() const = 0;
    NODISCARD virtual ImageFormat get_swapchain_image_format() const = 0;

    NODISCARD virtual void* get_swapchain_image(u32 image_index = 0) const = 0;
    NODISCARD virtual void* get_swapchain_image_view(u32 image_index = 0) const = 0;
};

} // namespace SE
