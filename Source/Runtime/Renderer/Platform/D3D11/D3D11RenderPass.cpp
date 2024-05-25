/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Renderer/Platform/D3D11/D3D11Framebuffer.h"
#include "Renderer/Platform/D3D11/D3D11Pipeline.h"
#include "Renderer/Platform/D3D11/D3D11Renderer.h"
#include "Renderer/Platform/D3D11/D3D11RenderPass.h"

namespace SE
{

D3D11RenderPass::D3D11RenderPass(const RenderPassInfo& info)
    : m_pipeline(info.pipeline.as<D3D11Pipeline>())
    , m_target_framebuffer(info.target.framebuffer.as<D3D11Framebuffer>())
    , m_should_clear_target(info.target.clear)
    , m_target_clear_color(info.target.clear_color)
{
}

D3D11RenderPass::~D3D11RenderPass()
{
}

} // namespace SE
