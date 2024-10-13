/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Renderer/Platform/D3D11/D3D11Framebuffer.h>
#include <Renderer/Platform/D3D11/D3D11Pipeline.h>
#include <Renderer/Platform/D3D11/D3D11RenderPass.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>
#include <Renderer/Platform/D3D11/D3D11Texture.h>

namespace SE
{

D3D11RenderPass::D3D11RenderPass(const RenderPassDescription& description)
    : m_description(description)
{
    if (m_description.target_framebuffer_attachments.count() != description.target_framebuffer->get_attachment_count())
    {
        SE_LOG_ERROR(""sv);
        SE_ASSERT(false);
    }
}

D3D11RenderPass::~D3D11RenderPass()
{
    m_input_textures.clear_and_shrink();
    m_input_texture_arrays.clear_and_shrink();

    m_description.target_framebuffer.release();
    m_description.pipeline.release();
}

bool D3D11RenderPass::bind_inputs()
{
    Vector<ID3D11ShaderResourceView*> fragment_shader_resource_views;
    Vector<ID3D11SamplerState*> fragment_shader_sampler_states;

    for (auto texture : m_input_textures)
    {
        fragment_shader_resource_views.add(texture.value->get_view_handle());
        fragment_shader_sampler_states.add(texture.value->get_sampler_state());
    }

    for (auto texture_array_bucket : m_input_texture_arrays)
    {
        Vector<RefPtr<D3D11Texture2D>>& texture_array = texture_array_bucket.value;

        // TODO: There shouldn't be one sampler per texture anyway.
        if (texture_array.has_elements())
            fragment_shader_sampler_states.add(texture_array.first()->get_sampler_state());

        for (RefPtr<D3D11Texture2D>& texture : texture_array)
        {
            if (texture.is_valid())
                fragment_shader_resource_views.add(texture->get_view_handle());
        }
    }

    D3D11Renderer::get_device_context()->PSSetShaderResources(0, (UINT)(fragment_shader_resource_views.count()), fragment_shader_resource_views.elements());
    D3D11Renderer::get_device_context()->PSSetSamplers(0, (UINT)(fragment_shader_sampler_states.count()), fragment_shader_sampler_states.elements());

    return true;
}

void D3D11RenderPass::set_input(StringView name, const RenderPassTextureBinding& texture_binding)
{
    SE_ASSERT(!m_input_textures.contains(name));
    m_input_textures.add(name, texture_binding.texture.as<D3D11Texture2D>());
}

void D3D11RenderPass::set_input(StringView name, const RenderPassTextureArrayBinding& texture_array_binding)
{
    SE_ASSERT(!m_input_texture_arrays.contains(name));

    Vector<RefPtr<D3D11Texture2D>> texture_array;
    texture_array.ensure_capacity(texture_array_binding.texture_array.count());

    for (const auto& texture : texture_array_binding.texture_array)
        texture_array.add(texture.as<D3D11Texture2D>());

    m_input_texture_arrays.add(name, move(texture_array));
}

void D3D11RenderPass::update_input(StringView name, RefPtr<Texture2D> texture)
{
    SE_ASSERT(m_input_textures.contains(name));
    m_input_textures.at(name) = texture.as<D3D11Texture2D>();
}

void D3D11RenderPass::update_input(StringView name, Span<RefPtr<Texture2D>> texture_array)
{
    SE_ASSERT(m_input_texture_arrays.contains(name));
    Vector<RefPtr<D3D11Texture2D>>& textures = m_input_texture_arrays.at(name);

    SE_ASSERT(texture_array.count() == textures.count());
    textures.clear();

    for (const auto& texture : texture_array)
        textures.add(texture.as<D3D11Texture2D>());
}

} // namespace SE
