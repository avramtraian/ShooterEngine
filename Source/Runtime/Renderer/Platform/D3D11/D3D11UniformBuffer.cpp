/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>
#include <Renderer/Platform/D3D11/D3D11UniformBuffer.h>

namespace SE
{

ALWAYS_INLINE static D3D11_USAGE get_uniform_buffer_usage(UniformBufferUsage usage)
{
    switch (usage)
    {
        case UniformBufferUsage::Immutable: return D3D11_USAGE_IMMUTABLE;
        case UniformBufferUsage::Default: return D3D11_USAGE_DEFAULT;
        case UniformBufferUsage::Dynamic: return D3D11_USAGE_DYNAMIC;
    }

    SE_LOG_TAG_ERROR("D3D11", "Invalid UniformBufferUsage!");
    SE_ASSERT(false);
    return D3D11_USAGE_DEFAULT;
}

D3D11UniformBuffer::D3D11UniformBuffer(const UniformBufferDescription& description)
    : m_buffer_handle(nullptr)
    , m_buffer_byte_count(description.byte_count)
    , m_usage(description.usage)
{
    if (m_usage == UniformBufferUsage::Immutable && description.data.is_empty())
    {
        SE_LOG_TAG_ERROR("D3D11", "Uniform buffer created with 'Immutable' usage but no initial data!");
        return;
    }

    UINT cpu_access_flags = {};
    if (m_usage == UniformBufferUsage::Dynamic)
    {
        // NOTE: The specification states that a uniform buffer created with `Dynamic` usage must
        // always have the `WRITE` bit in the CPU access flag set.
        cpu_access_flags |= D3D11_CPU_ACCESS_WRITE;
    }

    //
    // The specification of the uniform buffer.
    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc
    //
    D3D11_BUFFER_DESC buffer_description = {};
    buffer_description.ByteWidth = m_buffer_byte_count;
    buffer_description.Usage = get_uniform_buffer_usage(m_usage);
    buffer_description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_description.CPUAccessFlags = cpu_access_flags;

    // The data that is used to initialize the uniform buffer with.
    D3D11_SUBRESOURCE_DATA buffer_subresouce_data = {};
    buffer_subresouce_data.pSysMem = description.data.elements();
    D3D11_SUBRESOURCE_DATA* buffer_initial_data = description.data.has_elements() ? &buffer_subresouce_data : nullptr;

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateBuffer(&buffer_description, buffer_initial_data, &m_buffer_handle));
}

D3D11UniformBuffer::~D3D11UniformBuffer()
{
    SE_D3D11_RELEASE(m_buffer_handle);
}

void D3D11UniformBuffer::upload_data(ReadonlyByteSpan data)
{
    if (m_usage == UniformBufferUsage::Immutable)
    {
        SE_LOG_TAG_ERROR("D3D11", "Trying to upload data to a uniform buffer that was created with `Immutable` usage!");
        return;
    }

    if (data.is_empty())
        return;

    if (data.count() > m_buffer_byte_count)
    {
        SE_LOG_TAG_ERROR("D3D11", "Requested uploading '{}' bytes to a uniform buffer that has '{}' bytes!", data.count(), m_buffer_byte_count);
        return;
    }

    if (m_usage == UniformBufferUsage::Default)
    {
        //
        // The specification of the staging buffer that will be used to upload the data to the uniform buffer.
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc
        //
        D3D11_BUFFER_DESC staging_buffer_description = {};
        staging_buffer_description.ByteWidth = static_cast<UINT>(data.count());
        staging_buffer_description.Usage = D3D11_USAGE_STAGING;
        staging_buffer_description.BindFlags = 0;
        staging_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA staging_buffer_subresource_data = {};
        staging_buffer_subresource_data.pSysMem = data.elements();

        ID3D11Buffer* staging_buffer = nullptr;
        SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateBuffer(&staging_buffer_description, &staging_buffer_subresource_data, &staging_buffer));

        D3D11Renderer::get_device_context()->CopySubresourceRegion(m_buffer_handle, 0, 0, 0, 0, staging_buffer, 0, nullptr);
        SE_D3D11_RELEASE(staging_buffer);
    }
    else if (m_usage == UniformBufferUsage::Dynamic)
    {
        D3D11_MAPPED_SUBRESOURCE mapped_subresource = {};
        SE_D3D11_CHECK(D3D11Renderer::get_device_context()->Map(m_buffer_handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource));
        copy_memory_from_span(mapped_subresource.pData, data);
        D3D11Renderer::get_device_context()->Unmap(m_buffer_handle, 0);
    }
    else
    {
        // Should never be reached.
        SE_ASSERT(false);
    }
}

} // namespace SE
