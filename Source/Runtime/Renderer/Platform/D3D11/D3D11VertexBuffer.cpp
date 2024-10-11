/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>
#include <Renderer/Platform/D3D11/D3D11VertexBuffer.h>

namespace SE
{

ALWAYS_INLINE static D3D11_USAGE get_vertex_buffer_usage(VertexBufferUpdateFrequency update_frequency)
{
    switch (update_frequency)
    {
        case VertexBufferUpdateFrequency::Never: return D3D11_USAGE_IMMUTABLE;
        case VertexBufferUpdateFrequency::Normal: return D3D11_USAGE_DEFAULT;
        case VertexBufferUpdateFrequency::High: return D3D11_USAGE_DYNAMIC;
    }

    SE_LOG_TAG_ERROR("D3D11"sv, "Invalid VertexBufferUpdateFrequency!"sv);
    SE_ASSERT(false);
    return D3D11_USAGE_DEFAULT;
}

D3D11VertexBuffer::D3D11VertexBuffer(const VertexBufferInfo& info)
    : m_buffer(nullptr)
    , m_buffer_byte_count(info.byte_count)
    , m_update_frequency(info.update_frequency)
{
    if (m_update_frequency == VertexBufferUpdateFrequency::Never && info.data.is_empty())
    {
        SE_LOG_TAG_ERROR("D3D11"sv, "No data provided to vertex buffer flagged with VertexBufferUpdateFrequency::Never!"sv);
        return;
    }

    UINT cpu_access_flags = 0;
    if (m_update_frequency != VertexBufferUpdateFrequency::Never)
        cpu_access_flags |= D3D11_CPU_ACCESS_WRITE;

    D3D11_BUFFER_DESC buffer_description = {};
    buffer_description.ByteWidth = (UINT)(m_buffer_byte_count);
    buffer_description.Usage = get_vertex_buffer_usage(m_update_frequency);
    buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_description.CPUAccessFlags = cpu_access_flags;

    D3D11_SUBRESOURCE_DATA initial_data = {};
    D3D11_SUBRESOURCE_DATA* initial_data_ptr = nullptr;

    if (!info.data.is_empty())
    {
        SE_ASSERT(info.data.count() == m_buffer_byte_count);
        initial_data.pSysMem = info.data.elements();
        initial_data_ptr = &initial_data;
    }

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateBuffer(&buffer_description, initial_data_ptr, &m_buffer));
}

D3D11VertexBuffer::~D3D11VertexBuffer()
{
    SE_D3D11_RELEASE(m_buffer);
}

void D3D11VertexBuffer::update_data(ReadonlyByteSpan data)
{
    if (m_update_frequency == VertexBufferUpdateFrequency::Never)
    {
        SE_LOG_TAG_ERROR("D3D11"sv, "Trying to update the memory of a vertex buffer created with VertexBufferUpdateFrequency::Never!"sv);
        return;
    }

    if (m_update_frequency == VertexBufferUpdateFrequency::High)
    {
        // Map the buffer memory.
        D3D11_MAPPED_SUBRESOURCE buffer_subresource = {};
        SE_D3D11_CHECK(D3D11Renderer::get_device_context()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &buffer_subresource));
        // Copy data to the mapped memory.
        SE_ASSERT(data.count() <= m_buffer_byte_count);
        copy_memory_from_span(buffer_subresource.pData, data);
        // Unmap the buffer memory.
        D3D11Renderer::get_device_context()->Unmap(m_buffer, 0);
    }
    else
    {
        D3D11_BUFFER_DESC staging_buffer_description = {};
        staging_buffer_description.ByteWidth = (UINT)(data.count());
        staging_buffer_description.Usage = D3D11_USAGE_STAGING;
        staging_buffer_description.BindFlags = 0;
        staging_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA staging_buffer_data = {};
        staging_buffer_data.pSysMem = data.elements();

        ID3D11Buffer* staging_buffer = nullptr;
        SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateBuffer(&staging_buffer_description, &staging_buffer_data, &staging_buffer));

        // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_box
        D3D11_BOX copy_box = {};
        copy_box.left = 0;
        copy_box.right = (UINT)(data.count());
        // The Y-axis is pointing down.
        copy_box.top = 0;
        copy_box.bottom = 1;
        // The Z-axis is pointing away from the camera.
        copy_box.front = 0;
        copy_box.back = 1;
        D3D11Renderer::get_device_context()->CopySubresourceRegion(m_buffer, 0, 0, 0, 0, staging_buffer, 0, &copy_box);

        if (staging_buffer)
        {
            staging_buffer->Release();
            staging_buffer = nullptr;
        }
    }
}

} // namespace SE
