/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Framebuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Texture.h"

namespace SE
{

struct RenderPassTarget
{
    RefPtr<Framebuffer> framebuffer;
    bool clear = true;
    // Only used when the 'clear' flag is set to true.
    Color4 clear_color = { 0, 0, 0, 255 };
};

struct RenderPassInfo
{
    RefPtr<Pipeline> pipeline;
    RenderPassTarget target;
};

struct RenderPassTextureBinding
{
    RenderPassTextureBinding() = default;
    ALWAYS_INLINE explicit RenderPassTextureBinding(RefPtr<Texture2D> in_texture)
        : texture(move(in_texture))
    {}

    RefPtr<Texture2D> texture;
};

struct RenderPassTextureArrayBinding
{
    RenderPassTextureArrayBinding() = default;
    ALWAYS_INLINE explicit RenderPassTextureArrayBinding(Span<RefPtr<Texture2D>> in_texture_array)
        : texture_array(Vector<RefPtr<Texture2D>>::create_from_span(in_texture_array))
    {}

    Vector<RefPtr<Texture2D>> texture_array;
};

class RenderPass : public RefCounted
{
public:
    RenderPass() = default;
    virtual ~RenderPass() override = default;

    static RefPtr<RenderPass> create(const RenderPassInfo& info);

public:
    // Binds all the provided inputs to the pipeline. Must be manually invoked every time the render pass
    // begins or before one of the input resources will be used.
    virtual bool bind_inputs() = 0;

    virtual void set_input(StringView name, const RenderPassTextureBinding& texture_binding) = 0;
    virtual void set_input(StringView name, const RenderPassTextureArrayBinding& texture_array_binding) = 0;

    virtual void update_input(StringView name, RefPtr<Texture2D> texture) = 0;
    virtual void update_input(StringView name, Span<RefPtr<Texture2D>> texture_array) = 0;
};

} // namespace SE
