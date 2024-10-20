/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Renderer/Platform/D3D11/D3D11Headers.h>
#include <Renderer/UniformBuffer.h>

namespace SE
{

class D3D11UniformBuffer : public UniformBuffer
{
public:
    explicit D3D11UniformBuffer(const UniformBufferDescription& description);
    virtual ~D3D11UniformBuffer() override;

    NODISCARD ALWAYS_INLINE ID3D11Buffer* get_handle() const { return m_buffer_handle; }

public:
    virtual void upload_data(ReadonlyByteSpan data) override;

private:
    ID3D11Buffer* m_buffer_handle;
    u32 m_buffer_byte_count;
    UniformBufferUsage m_usage;
};

} // namespace SE
