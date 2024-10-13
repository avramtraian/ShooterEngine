/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Renderer/Platform/D3D11/D3D11IndexBuffer.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>

namespace SE
{

static u32 get_index_type_size(IndexType index_type)
{
    switch (index_type)
    {
        case IndexType::UInt16: return 2;
        case IndexType::UInt32: return 4;
    }

    SE_LOG_TAG_ERROR("D3D11"sv, "Invalid IndexType!"sv);
    SE_ASSERT(false);
    return 0;
}

D3D11IndexBuffer::D3D11IndexBuffer(const IndexBufferDescription& description)
    : m_handle(nullptr)
    , m_index_type(description.index_type)
{
    D3D11_BUFFER_DESC buffer_description = {};
    buffer_description.ByteWidth = static_cast<UINT>(description.byte_count);
    buffer_description.Usage = D3D11_USAGE_IMMUTABLE;
    buffer_description.BindFlags = D3D11_BIND_INDEX_BUFFER;
    buffer_description.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initial_data = {};
    if (!description.data.is_empty())
    {
        SE_ASSERT(description.data.count() == description.byte_count);
        initial_data.pSysMem = description.data.elements();
    }

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateBuffer(&buffer_description, &initial_data, &m_handle));
}

D3D11IndexBuffer::~D3D11IndexBuffer()
{
    SE_D3D11_RELEASE(m_handle);
}

} // namespace SE
