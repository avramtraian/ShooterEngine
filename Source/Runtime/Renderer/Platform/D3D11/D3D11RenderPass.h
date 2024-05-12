/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Platform/D3D11/D3D11Headers.h"
#include "Renderer/RenderPass.h"

namespace SE
{

// Forward declarations.
class D3D11Pipeline;
class D3D11Framebuffer;

class D3D11RenderPass final : public RenderPass
{
public:
    D3D11RenderPass(const RenderPassInfo& info);
    virtual ~D3D11RenderPass() override;

    ALWAYS_INLINE RefPtr<D3D11Pipeline>& get_pipeline() { return m_pipeline; }
    ALWAYS_INLINE RefPtr<D3D11Framebuffer>& get_target_framebuffer() { return m_target_framebuffer; }

private:
    RefPtr<D3D11Pipeline> m_pipeline;
    RefPtr<D3D11Framebuffer> m_target_framebuffer;
};

} // namespace SE