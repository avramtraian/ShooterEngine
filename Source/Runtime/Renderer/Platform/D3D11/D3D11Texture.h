/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Platform/D3D11/D3D11Headers.h"
#include "Renderer/Texture.h"

namespace SE
{

class D3D11Texture2D final : public Texture2D
{
public:
    D3D11Texture2D(const Texture2DInfo& info);
    virtual ~D3D11Texture2D() override;

    ALWAYS_INLINE virtual u32 get_width() const { return m_width; }
    ALWAYS_INLINE virtual u32 get_height() const { return m_height; }
    ALWAYS_INLINE virtual ImageFormat get_format() const { return m_format; }

    NODISCARD ALWAYS_INLINE ID3D11Texture2D* get_image_handle() const { return m_image_handle; }
    NODISCARD ALWAYS_INLINE ID3D11ShaderResourceView* get_shader_resource_view() const { return m_image_view; }
    NODISCARD ALWAYS_INLINE ID3D11SamplerState* get_sampler_state() const { return m_sampler_state; }

private:
    ID3D11Texture2D* m_image_handle;
    ID3D11ShaderResourceView* m_image_view;
    ID3D11SamplerState* m_sampler_state;

    u32 m_width;
    u32 m_height;
    ImageFormat m_format;
};

} // namespace SE
