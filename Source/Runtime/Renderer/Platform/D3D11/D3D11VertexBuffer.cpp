/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Renderer/Platform/D3D11/D3D11Renderer.h"
#include "Renderer/Platform/D3D11/D3D11VertexBuffer.h"

namespace SE
{

D3D11VertexBuffer::D3D11VertexBuffer(const VertexBufferInfo& info)
    : m_buffer(nullptr)
{
    D3D11_BUFFER_DESC buffer_description = {};
    buffer_description.ByteWidth = (UINT)(info.byte_count);
    buffer_description.Usage = D3D11_USAGE_IMMUTABLE;
    buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_description.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initial_data = {};
    if (!info.data.is_empty())
    {
        SE_ASSERT(info.data.count() == info.byte_count);
        initial_data.pSysMem = info.data.elements();
    }

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateBuffer(&buffer_description, &initial_data, &m_buffer));
}

D3D11VertexBuffer::~D3D11VertexBuffer()
{
    if (m_buffer != nullptr)
    {
        m_buffer->Release();
        m_buffer = nullptr;
    }
}

} // namespace SE