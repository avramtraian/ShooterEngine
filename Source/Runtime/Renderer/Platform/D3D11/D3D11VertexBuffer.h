/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Renderer/Platform/D3D11/D3D11Headers.h>
#include <Renderer/VertexBuffer.h>

namespace SE
{

class D3D11VertexBuffer : public VertexBuffer
{
public:
    explicit D3D11VertexBuffer(const VertexBufferDescription& description);
    virtual ~D3D11VertexBuffer() override;

    ALWAYS_INLINE ID3D11Buffer* get_handle() const { return m_handle; }

public:
    virtual void update_data(ReadonlyByteSpan data) override;

private:
    ID3D11Buffer* m_handle;
    u32 m_buffer_byte_count;
    VertexBufferUpdateFrequency m_update_frequency;
};

} // namespace SE
