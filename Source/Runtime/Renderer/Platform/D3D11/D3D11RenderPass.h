/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/HashMap.h>
#include <Renderer/Platform/D3D11/D3D11Headers.h>
#include <Renderer/RenderPass.h>

namespace SE
{

// Forward declarations.
class D3D11Pipeline;
class D3D11Framebuffer;
class D3D11Texture2D;
class D3D11UniformBuffer;

class D3D11RenderPass : public RenderPass
{
public:
    explicit D3D11RenderPass(const RenderPassDescription& description);
    virtual ~D3D11RenderPass() override;

    NODISCARD ALWAYS_INLINE RefPtr<D3D11Pipeline> get_pipeline() { return m_description.pipeline.as<D3D11Pipeline>(); }
    NODISCARD ALWAYS_INLINE const RefPtr<const D3D11Pipeline> get_pipeline() const { return m_description.pipeline.as<const D3D11Pipeline>(); }

    NODISCARD ALWAYS_INLINE RefPtr<D3D11Framebuffer> get_target_framebuffer() { return m_description.target_framebuffer.as<D3D11Framebuffer>(); }
    NODISCARD ALWAYS_INLINE const RefPtr<const D3D11Framebuffer> get_target_framebuffer() const
    {
        return m_description.target_framebuffer.as<const D3D11Framebuffer>();
    }

    // Returns the render pass attachment description corresponding to the given framebuffer attachment index.
    NODISCARD ALWAYS_INLINE const RenderPassAttachmentDescription& get_attachment_description(u32 attachment_index) const
    {
        SE_ASSERT(attachment_index < m_description.target_framebuffer_attachments.count());
        return m_description.target_framebuffer_attachments[attachment_index];
    }

public:
    virtual bool bind_inputs() override;

    virtual void set_input(StringView name, const RenderPassUniformBufferBinding& uniform_buffer_binding) override;
    virtual void set_input(StringView name, const RenderPassTextureBinding& texture_binding) override;
    virtual void set_input(StringView name, const RenderPassTextureArrayBinding& texture_array_binding) override;

    virtual void update_input(StringView name, RefPtr<UniformBuffer> uniform_buffer) override;
    virtual void update_input(StringView name, RefPtr<Texture2D> texture) override;
    virtual void update_input(StringView name, Span<RefPtr<Texture2D>> texture_array) override;

private:
    RenderPassDescription m_description;

    HashMap<String, RenderPassUniformBufferBinding> m_input_uniform_buffers;
    HashMap<String, RefPtr<D3D11Texture2D>> m_input_textures;
    HashMap<String, Vector<RefPtr<D3D11Texture2D>>> m_input_texture_arrays;
};

} // namespace SE
