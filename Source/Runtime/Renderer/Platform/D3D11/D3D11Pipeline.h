/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Pipeline.h"
#include "Renderer/Platform/D3D11/D3D11Headers.h"

namespace SE
{

// Forward declarations.
class D3D11Shader;

class D3D11Pipeline final : public Pipeline
{
public:
    D3D11Pipeline(const PipelineInfo& info);
    virtual ~D3D11Pipeline() override;

    ALWAYS_INLINE ID3D11InputLayout* get_input_layout() const { return m_input_layout; }
    ALWAYS_INLINE ID3D11RasterizerState* get_rasterizer_state() const { return m_rasterizer_state; }

    ALWAYS_INLINE const PipelineLayout& get_layout() const { return m_layout; }
    ALWAYS_INLINE u32 get_vertex_stride() const { return m_vertex_stride; }
    ALWAYS_INLINE RefPtr<D3D11Shader> get_shader() const { return m_shader; }
    ALWAYS_INLINE PrimitiveTopology get_primitive_topology() const { return m_primitive_topology; }
    ALWAYS_INLINE FrontFace get_front_face() const { return m_front_face; }
    ALWAYS_INLINE bool is_culling_enabled() const { return m_is_culling_enabled; }

private:
    ID3D11InputLayout* m_input_layout;
    ID3D11RasterizerState* m_rasterizer_state;

    PipelineLayout m_layout;
    u32 m_vertex_stride;
    RefPtr<D3D11Shader> m_shader;
    PrimitiveTopology m_primitive_topology;
    FrontFace m_front_face;
    bool m_is_culling_enabled;
};

} // namespace SE
