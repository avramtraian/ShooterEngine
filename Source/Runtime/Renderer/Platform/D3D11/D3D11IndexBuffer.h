/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Renderer/IndexBuffer.h>
#include <Renderer/Platform/D3D11/D3D11Headers.h>

namespace SE
{

class D3D11IndexBuffer : public IndexBuffer
{
public:
    explicit D3D11IndexBuffer(const IndexBufferDescription& description);
    virtual ~D3D11IndexBuffer() override;

    ALWAYS_INLINE ID3D11Buffer* get_handle() const { return m_handle; }
    ALWAYS_INLINE IndexType get_index_type() const { return m_index_type; }

private:
    ID3D11Buffer* m_handle;
    IndexType m_index_type;
};

} // namespace SE
