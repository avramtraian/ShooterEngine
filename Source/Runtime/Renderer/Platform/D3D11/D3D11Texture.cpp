/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Renderer/Platform/D3D11/D3D11Image.h"
#include "Renderer/Platform/D3D11/D3D11Renderer.h"
#include "Renderer/Platform/D3D11/D3D11Texture.h"

namespace SE
{

D3D11Texture2D::D3D11Texture2D(const Texture2DInfo& info)
    : m_image_handle(nullptr)
    , m_image_view(nullptr)
    , m_width(info.width)
    , m_height(info.height)
    , m_format(info.format)
{
    D3D11_TEXTURE2D_DESC texture_description = {};
    texture_description.Width = (UINT)(m_width);
    texture_description.Height = (UINT)(m_height);
    texture_description.MipLevels = 1;
    texture_description.ArraySize = 1;
    texture_description.Format = d3d11_image_format(m_format);
    texture_description.SampleDesc.Count = 1;
    texture_description.SampleDesc.Quality = 0;
    texture_description.Usage = D3D11_USAGE_DEFAULT;
    texture_description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texture_description.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initial_data = {};
    if (!info.data.is_empty())
    {
        const u32 slice_pitch = m_width * d3d11_image_format_size(m_format);
        // The provided data buffer doesn't match the texture number of bytes.
        SE_ASSERT((usize)(slice_pitch) * (usize)(m_height) == info.data.count());

        initial_data.pSysMem = info.data.elements();
        initial_data.SysMemPitch = slice_pitch;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC texture_view_description = {};
    texture_view_description.Format = d3d11_image_format(m_format);
    texture_view_description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    texture_view_description.Texture2D.MipLevels = 1;
    texture_view_description.Texture2D.MostDetailedMip = 0;

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateTexture2D(&texture_description, &initial_data, &m_image_handle));
    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateShaderResourceView(m_image_handle, &texture_view_description, &m_image_view));
}

D3D11Texture2D::~D3D11Texture2D()
{
    if (m_image_view)
    {
        m_image_view->Release();
        m_image_view = nullptr;
    }

    if (m_image_handle)
    {
        m_image_handle->Release();
        m_image_handle = nullptr;
    }
}

} // namespace SE
