/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/Containers/HashTable.h"
#include "Renderer/Platform/D3D11/D3D11Pipeline.h"
#include "Renderer/Platform/D3D11/D3D11Renderer.h"
#include "Renderer/Platform/D3D11/D3D11Shader.h"

namespace SE
{

struct VertexAttributeMetadata
{
    u32         size;
    DXGI_FORMAT format;
    u8          semantic_index_count;
};

ALWAYS_INLINE static VertexAttributeMetadata get_vertex_attribute_metadata(VertexAttribute::Type attribute_type)
{
    switch (attribute_type)
    {
        case VertexAttribute::Float1: return { 4 * 1,  DXGI_FORMAT_R32_FLOAT,          1 };
        case VertexAttribute::Float2: return { 4 * 2,  DXGI_FORMAT_R32G32_FLOAT,       1 };
        case VertexAttribute::Float3: return { 4 * 3,  DXGI_FORMAT_R32G32B32_FLOAT,    1 };
        case VertexAttribute::Float4: return { 4 * 4,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1 };
        case VertexAttribute::Int1:   return { 4 * 1,  DXGI_FORMAT_R32_SINT,           1 };
        case VertexAttribute::Int2:   return { 4 * 2,  DXGI_FORMAT_R32G32_SINT,        1 };
        case VertexAttribute::Int3:   return { 4 * 3,  DXGI_FORMAT_R32G32B32_SINT,     1 };
        case VertexAttribute::Int4:   return { 4 * 4,  DXGI_FORMAT_R32G32B32A32_SINT,  1 };
        case VertexAttribute::UInt1:  return { 4 * 1,  DXGI_FORMAT_R32_UINT,           1 };
        case VertexAttribute::UInt2:  return { 4 * 2,  DXGI_FORMAT_R32G32_UINT,        1 };
        case VertexAttribute::UInt3:  return { 4 * 3,  DXGI_FORMAT_R32G32B32_UINT,     1 };
        case VertexAttribute::UInt4:  return { 4 * 4,  DXGI_FORMAT_R32G32B32A32_UINT,  1 };
        case VertexAttribute::Mat2:   return { 4 * 2,  DXGI_FORMAT_R32G32_FLOAT,       2 };
        case VertexAttribute::Mat3:   return { 4 * 3,  DXGI_FORMAT_R32G32B32_FLOAT,    3 };
        case VertexAttribute::Mat4:   return { 4 * 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 4 };
    }

    SE_LOG_TAG_ERROR("D3D11"sv, "Invalid VertexAttribute::Type!"sv);
    SE_ASSERT(false);
    return {};
}

D3D11Pipeline::D3D11Pipeline(const PipelineInfo& info)
    : m_input_layout(nullptr)
    , m_rasterizer_state(nullptr)
    , m_layout(info.layout)
    , m_vertex_stride(0)
    , m_shader(info.shader.as<D3D11Shader>())
    , m_primitive_topology(info.primitive_topology)
    , m_front_face(info.front_face)
    , m_is_culling_enabled(info.enable_culling)
{
    Vector<D3D11_INPUT_ELEMENT_DESC> input_element_descriptions;
    input_element_descriptions.ensure_capacity(m_layout.attributes.count());

    HashTable<StringView> used_semantic_names;
    u32 attribute_offset = 0;

    for (const VertexAttribute& vertex_attribute : m_layout.attributes)
    {
        if (used_semantic_names.contains(vertex_attribute.name.view()))
        {
            SE_LOG_TAG_ERROR("D3D11"sv, "Semantic name '{}' already exists!"sv, vertex_attribute.name);
            SE_ASSERT(false);
        }
        used_semantic_names.add(vertex_attribute.name.view());

        const VertexAttributeMetadata attribute_metadata = get_vertex_attribute_metadata(vertex_attribute.type);

        D3D11_INPUT_ELEMENT_DESC element_description = {};
        element_description.SemanticName = vertex_attribute.name.byte_span().as<const char>().elements();
        element_description.Format = attribute_metadata.format;
        element_description.InputSlot = 0;
        element_description.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        element_description.InstanceDataStepRate = 0;

        for (u32 semantic_index = 0; semantic_index < attribute_metadata.semantic_index_count; ++semantic_index)
        {
            element_description.SemanticIndex = semantic_index;
            element_description.AlignedByteOffset = attribute_offset;

            input_element_descriptions.add(element_description);
            attribute_offset += attribute_metadata.size;
        }
    }

    m_vertex_stride = attribute_offset;

    Optional<ReadonlyByteSpan> vertex_shader_bytecode = m_shader->get_bytecode();
    SE_ASSERT(vertex_shader_bytecode.has_value());

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateInputLayout(
        input_element_descriptions.elements(), (UINT)(input_element_descriptions.count()),
        vertex_shader_bytecode->elements(), vertex_shader_bytecode->count(),
        &m_input_layout)
    );

    D3D11_RASTERIZER_DESC rasterizer_description = {};
    rasterizer_description.FillMode = D3D11_FILL_SOLID;
    rasterizer_description.CullMode = m_is_culling_enabled ? D3D11_CULL_BACK : D3D11_CULL_NONE;
    rasterizer_description.FrontCounterClockwise = (m_front_face == FrontFace::CounterClockwise);
    rasterizer_description.DepthClipEnable = false;

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateRasterizerState(&rasterizer_description, &m_rasterizer_state));
}

D3D11Pipeline::~D3D11Pipeline()
{
    if (m_input_layout)
    {
        m_input_layout->Release();
        m_input_layout = nullptr;
    }

    if (m_rasterizer_state)
    {
        m_rasterizer_state->Release();
        m_rasterizer_state = nullptr;
    }
}

} // namespace SE
