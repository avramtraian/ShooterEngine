/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Platform/D3D11/D3D11Context.h"
#include "Renderer/Platform/D3D11/D3D11Headers.h"
#include "Renderer/RendererInterface.h"

#define SE_D3D11_CHECK(expression)                                                      \
    {                                                                                   \
        const HRESULT _result = expression;                                             \
        if (FAILED(_result))                                                            \
        {                                                                               \
            const String error_code_string = D3D11Renderer::get_error_message(_result); \
            SE_LOG_TAG_ERROR("D3D11"sv,                                                 \
                             "\n    {}\n    Failed with error code: {}"sv,              \
                             #expression##sv, error_code_string);                       \
            SE_ASSERT(false);                                                           \
        }                                                                               \
    }

namespace SE
{

class D3D11Renderer final : public RendererInterface
{
public:
    virtual bool initialize() override;
    virtual void shutdown() override;

    static String get_error_message(HRESULT result);

    static ID3D11Device* get_device();
    static ID3D11DeviceContext* get_device_context();
    static IDXGIFactory2* get_dxgi_factory();
    static D3D11Context* get_active_context();

public:
    virtual void begin_render_pass(RefPtr<RenderPass> render_pass) override;
    virtual void end_render_pass() override;
};

} // namespace SE