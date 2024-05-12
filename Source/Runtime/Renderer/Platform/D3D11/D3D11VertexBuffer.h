/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Platform/D3D11/D3D11Headers.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/VertexBuffer.h"

namespace SE
{

class D3D11VertexBuffer final : public VertexBuffer
{
public:
    D3D11VertexBuffer(const VertexBufferInfo& info);
    virtual ~D3D11VertexBuffer() override;

    ALWAYS_INLINE ID3D11Buffer* get_handle() const { return m_buffer; }

private:
    ID3D11Buffer* m_buffer;
};

} // namespace SE