/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Renderer/Platform/D3D11/D3D11Image.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>
#include <Renderer/Platform/D3D11/D3D11Texture.h>

namespace SE
{

D3D11Texture2D::D3D11Texture2D(const Texture2DDescription& description)
    : m_handle(nullptr)
    , m_view_handle(nullptr)
    , m_sampler_state(nullptr)
    , m_width(description.width)
    , m_height(description.height)
    , m_format(description.format)
    , m_min_filter(description.min_filter)
    , m_mag_filter(description.mag_filter)
    , m_address_mode_u(description.address_mode_u)
    , m_address_mode_v(description.address_mode_v)
    , m_address_mode_w(description.address_mode_w)
{
    //
    // The specification of the texture.
    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_texture2d_desc
    //
    D3D11_TEXTURE2D_DESC texture_description = {};
    texture_description.Width = static_cast<UINT>(m_width);
    texture_description.Height = static_cast<UINT>(m_height);
    texture_description.MipLevels = 1;
    texture_description.ArraySize = 1;
    texture_description.Format = get_d3d11_image_format(m_format);
    texture_description.SampleDesc.Count = 1;
    texture_description.SampleDesc.Quality = 0;
    texture_description.Usage = D3D11_USAGE_DEFAULT;
    texture_description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texture_description.CPUAccessFlags = 0;

    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_subresource_data
    D3D11_SUBRESOURCE_DATA initial_data = {};
    if (!description.data.is_empty())
    {
        const u32 slice_pitch = m_width * get_d3d11_image_format_size(m_format);
        // The provided data buffer doesn't match the texture number of bytes.
        SE_ASSERT(static_cast<usize>(slice_pitch) * static_cast<usize>(m_height) == description.data.count());

        initial_data.pSysMem = description.data.elements();
        initial_data.SysMemPitch = slice_pitch;
    }

    //
    // The specification of the texture view (shader resource view).
    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_shader_resource_view_desc
    //
    D3D11_SHADER_RESOURCE_VIEW_DESC texture_view_description = {};
    texture_view_description.Format = get_d3d11_image_format(m_format);
    texture_view_description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    texture_view_description.Texture2D.MipLevels = 1;
    texture_view_description.Texture2D.MostDetailedMip = 0;

    //
    // The specification of the sampler that is used to sample the texture.
    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_sampler_desc
    //
    D3D11_SAMPLER_DESC sampler_description = {};
    sampler_description.Filter = get_d3d11_image_filtering_mode(m_min_filter, m_mag_filter);
    sampler_description.AddressU = get_d3d11_image_address_mode(m_address_mode_u);
    sampler_description.AddressV = get_d3d11_image_address_mode(m_address_mode_v);
    sampler_description.AddressW = get_d3d11_image_address_mode(m_address_mode_w);

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateTexture2D(&texture_description, &initial_data, &m_handle));
    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateShaderResourceView(m_handle, &texture_view_description, &m_view_handle));
    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateSamplerState(&sampler_description, &m_sampler_state));
}

D3D11Texture2D::~D3D11Texture2D()
{
    SE_D3D11_RELEASE(m_sampler_state);
    SE_D3D11_RELEASE(m_view_handle);
    SE_D3D11_RELEASE(m_handle);
}

} // namespace SE
