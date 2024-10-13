/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Renderer/Pipeline.h>
#include <Renderer/Platform/D3D11/D3D11Headers.h>

namespace SE
{

class D3D11Pipeline final : public Pipeline
{
public:
    D3D11Pipeline(const PipelineDescription& description);
    virtual ~D3D11Pipeline() override;

    NODISCARD ALWAYS_INLINE ID3D11InputLayout* get_input_layout() const { return m_input_layout; }
    NODISCARD ALWAYS_INLINE ID3D11RasterizerState* get_rasterizer_state() const { return m_rasterizer_state; }
    NODISCARD ALWAYS_INLINE u32 get_vertex_stride() const { return m_vertex_stride; }

    NODISCARD ALWAYS_INLINE virtual RefPtr<Shader> get_shader() const override { return m_description.shader; }
    NODISCARD ALWAYS_INLINE virtual PipelinePrimitiveTopology get_primitive_topology() const override { return m_description.primitive_topology; }
    NODISCARD ALWAYS_INLINE virtual PipelineFillMode get_fill_mode() const override { return m_description.fill_mode; }
    NODISCARD ALWAYS_INLINE virtual PipelineCullMode get_cull_mode() const override { return m_description.cull_mode; }
    NODISCARD ALWAYS_INLINE virtual PipelineFrontFaceDirection get_front_face_direction() const override { return m_description.front_face_direction; }

private:
    ID3D11InputLayout* m_input_layout;
    ID3D11RasterizerState* m_rasterizer_state;
    PipelineDescription m_description;
    u32 m_vertex_stride;
};

} // namespace SE
