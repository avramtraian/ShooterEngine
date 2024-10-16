/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/RefPtr.h>
#include <Core/Containers/StringView.h>
#include <Core/Containers/Vector.h>
#include <Core/Math/Color.h>
#include <Renderer/Framebuffer.h>
#include <Renderer/Pipeline.h>
#include <Renderer/Texture.h>

namespace SE
{

enum class RenderPassAttachmentLoadOperation : u8
{
    Unknown = 0,
    Load,
    Clear,
    DontCare,
};

enum class RenderPassAttachmentStoreOperation : u8
{
    Unknown = 0,
    Store,
    DontCare,
};

struct RenderPassAttachmentDescription
{
    RenderPassAttachmentLoadOperation load_operation { RenderPassAttachmentLoadOperation::DontCare };
    RenderPassAttachmentStoreOperation store_operation { RenderPassAttachmentStoreOperation::Store };
    
    // Only used when `load_operation` is set to `RenderPassAttachmentLoadOperation::Clear`.
    Color4 clear_color { 0, 0, 0, 0 };
};

struct RenderPassDescription
{
    RefPtr<Pipeline> pipeline;
    RefPtr<Framebuffer> target_framebuffer;
    Vector<RenderPassAttachmentDescription> target_framebuffer_attachments;
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
    NODISCARD SHOOTER_API static RefPtr<RenderPass> create(const RenderPassDescription& description);

    RenderPass() = default;
    virtual ~RenderPass() override = default;

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
