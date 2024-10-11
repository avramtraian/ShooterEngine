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

class D3D11RenderPass final : public RenderPass
{
public:
    D3D11RenderPass(const RenderPassInfo& info);
    virtual ~D3D11RenderPass() override;

    ALWAYS_INLINE RefPtr<D3D11Pipeline>& get_pipeline() { return m_pipeline; }

    ALWAYS_INLINE RefPtr<D3D11Framebuffer>& get_target_framebuffer() { return m_target_framebuffer; }
    ALWAYS_INLINE bool should_clear_target() const { return m_should_clear_target; }
    ALWAYS_INLINE Color4 get_target_clear_color() const { return m_target_clear_color; }

public:
    virtual bool bind_inputs() override;

    virtual void set_input(StringView name, const RenderPassTextureBinding& texture_binding) override;
    virtual void set_input(StringView name, const RenderPassTextureArrayBinding& texture_array_binding) override;

    virtual void update_input(StringView name, RefPtr<Texture2D> texture) override;
    virtual void update_input(StringView name, Span<RefPtr<Texture2D>> texture_array) override;

private:
    RefPtr<D3D11Pipeline> m_pipeline;
    RefPtr<D3D11Framebuffer> m_target_framebuffer;
    bool m_should_clear_target;
    Color4 m_target_clear_color;

    HashMap<String, RefPtr<D3D11Texture2D>> m_input_textures;
    HashMap<String, Vector<RefPtr<D3D11Texture2D>>> m_input_texture_arrays;
};

} // namespace SE
