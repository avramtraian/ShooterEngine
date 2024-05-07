/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Renderer/Platform/D3D11/D3D11Renderer.h"

#include <comdef.h>
#include <system_error>

namespace SE
{

struct D3D11RendererData
{
    ID3D11Device* device;
    D3D_FEATURE_LEVEL device_feature_level;
    IDXGIFactory* dxgi_factory;
};

static OwnPtr<D3D11RendererData> s_d3d11_renderer;

bool D3D11Renderer::initialize()
{
    if (s_d3d11_renderer.is_valid())
        return false;
    s_d3d11_renderer = make_own<D3D11RendererData>();

    UINT device_flags = 0;
#if SE_CONFIGURATION_DEBUG
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // SE_CONFIGURATION_DEBUG

    const D3D_FEATURE_LEVEL feature_levels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    const HRESULT device_result = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, device_flags,
        feature_levels, SE_ARRAY_COUNT(feature_levels), D3D11_SDK_VERSION,
        &s_d3d11_renderer->device, &s_d3d11_renderer->device_feature_level, nullptr);
    
    if (device_result != S_OK)
    {
        SE_LOG_TAG_ERROR("D3D11"sv, "Failed to create the device!"sv);
        return false;
    }

    const HRESULT dxgi_factory_result = CreateDXGIFactory(IID_PPV_ARGS(&s_d3d11_renderer->dxgi_factory));
    if (dxgi_factory_result != S_OK)
    {
        SE_LOG_TAG_ERROR("D3D11"sv, "Failed to create the DXGI factory!"sv);
        return false;
    }

    return true;
}

void D3D11Renderer::shutdown()
{
    if (!s_d3d11_renderer.is_valid())
        return;

    s_d3d11_renderer->dxgi_factory->Release();
    s_d3d11_renderer->dxgi_factory = nullptr;

    s_d3d11_renderer->device->Release();
    s_d3d11_renderer->device = nullptr;

    s_d3d11_renderer.release();
}

String D3D11Renderer::get_error_message(HRESULT result)
{
    std::string error_message = std::system_category().message(result);
    return StringView::create_from_utf8(error_message.c_str(), error_message.size() - 1);
}

ID3D11Device* D3D11Renderer::get_device()
{
    SE_ASSERT_DEBUG(s_d3d11_renderer->device != nullptr);
    return s_d3d11_renderer->device;
}

IDXGIFactory* D3D11Renderer::get_dxgi_factory()
{
    SE_ASSERT_DEBUG(s_d3d11_renderer->dxgi_factory != nullptr);
    return s_d3d11_renderer->dxgi_factory;
}

} // namespace SE