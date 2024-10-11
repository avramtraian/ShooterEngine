/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Log.h>
#include <Renderer/Image.h>
#include <Renderer/Platform/D3D11/D3D11Headers.h>

namespace SE
{

NODISCARD ALWAYS_INLINE constexpr DXGI_FORMAT d3d11_image_format(ImageFormat image_format)
{
    switch (image_format)
    {
        case ImageFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
        case ImageFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::BGRA8: return DXGI_FORMAT_B8G8R8A8_UNORM;
    }

    SE_ASSERT(false);
    return DXGI_FORMAT_UNKNOWN;
}

NODISCARD ALWAYS_INLINE u32 d3d11_image_format_size(ImageFormat image_format)
{
    switch (image_format)
    {
        case ImageFormat::Unknown: return 0;
        case ImageFormat::RGBA8: return 4;
        case ImageFormat::BGRA8: return 4;
    }

    SE_ASSERT(false);
    return 0;
}

NODISCARD ALWAYS_INLINE constexpr D3D11_FILTER d3d11_image_filtering(ImageFiltering min_image_filtering, ImageFiltering mag_image_filtering)
{
    switch (min_image_filtering)
    {
        case ImageFiltering::Linear:
        {
            switch (mag_image_filtering)
            {
                case ImageFiltering::Linear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                case ImageFiltering::Nearest: return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            }

            break;
        }

        case ImageFiltering::Nearest:
        {
            switch (mag_image_filtering)
            {
                case ImageFiltering::Linear: return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
                case ImageFiltering::Nearest: return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
            }

            break;
        }
    }

    SE_ASSERT(false);
    return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
}

NODISCARD ALWAYS_INLINE constexpr D3D11_TEXTURE_ADDRESS_MODE d3d11_image_address_mode(ImageAddressMode image_address_mode)
{
    switch (image_address_mode)
    {
        case ImageAddressMode::ClampToEdge: return D3D11_TEXTURE_ADDRESS_CLAMP;
        case ImageAddressMode::Repeat: return D3D11_TEXTURE_ADDRESS_WRAP;
        case ImageAddressMode::MirrorRepeat: return D3D11_TEXTURE_ADDRESS_MIRROR;
    }

    SE_ASSERT(false);
    return D3D11_TEXTURE_ADDRESS_CLAMP;
}

} // namespace SE
