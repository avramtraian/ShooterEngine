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

private:
    ID3D11Texture2D* m_image_handle;
    ID3D11ShaderResourceView* m_image_view;
    u32 m_width;
    u32 m_height;
    ImageFormat m_format;
};

} // namespace SE
