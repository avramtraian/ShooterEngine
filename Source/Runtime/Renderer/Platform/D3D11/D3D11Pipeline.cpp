/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Renderer/Platform/D3D11/D3D11Pipeline.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>
#include <Renderer/Platform/D3D11/D3D11Shader.h>

namespace SE
{

NODISCARD ALWAYS_INLINE static u32 get_pipeline_vertex_attribute_type_size(PipelineVertexAttributeType attribute_type)
{
    switch (attribute_type)
    {
        case PipelineVertexAttributeType::Float1: return 4;
        case PipelineVertexAttributeType::Float2: return 8;
        case PipelineVertexAttributeType::Float3: return 12;
        case PipelineVertexAttributeType::Float4: return 16;

        case PipelineVertexAttributeType::Int1: return 4;
        case PipelineVertexAttributeType::Int2: return 8;
        case PipelineVertexAttributeType::Int3: return 12;
        case PipelineVertexAttributeType::Int4: return 16;

        case PipelineVertexAttributeType::UInt1: return 4;
        case PipelineVertexAttributeType::UInt2: return 8;
        case PipelineVertexAttributeType::UInt3: return 12;
        case PipelineVertexAttributeType::UInt4: return 16;
    }

    SE_ASSERT(false);
    return 0;
}

NODISCARD ALWAYS_INLINE static DXGI_FORMAT get_pipeline_vertex_attribute_type_format(PipelineVertexAttributeType attribute_type)
{
    switch (attribute_type)
    {
        case PipelineVertexAttributeType::Float1: return DXGI_FORMAT_R32_FLOAT;
        case PipelineVertexAttributeType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
        case PipelineVertexAttributeType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
        case PipelineVertexAttributeType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case PipelineVertexAttributeType::Int1: return DXGI_FORMAT_R32_SINT;
        case PipelineVertexAttributeType::Int2: return DXGI_FORMAT_R32G32_SINT;
        case PipelineVertexAttributeType::Int3: return DXGI_FORMAT_R32G32B32_SINT;
        case PipelineVertexAttributeType::Int4: return DXGI_FORMAT_R32G32B32A32_SINT;

        case PipelineVertexAttributeType::UInt1: return DXGI_FORMAT_R32_UINT;
        case PipelineVertexAttributeType::UInt2: return DXGI_FORMAT_R32G32_UINT;
        case PipelineVertexAttributeType::UInt3: return DXGI_FORMAT_R32G32B32_UINT;
        case PipelineVertexAttributeType::UInt4: return DXGI_FORMAT_R32G32B32A32_UINT;
    }

    SE_ASSERT(false);
    return DXGI_FORMAT_UNKNOWN;
}

D3D11Pipeline::D3D11Pipeline(const PipelineDescription& description)
    : m_input_layout(nullptr)
    , m_rasterizer_state(nullptr)
    , m_description(description)
    , m_vertex_stride(0)
{
    if (m_description.vertex_attributes.has_elements())
    {
        Vector<D3D11_INPUT_ELEMENT_DESC> input_element_descriptions;
        input_element_descriptions.ensure_capacity(m_description.vertex_attributes.count());

        u32 vertex_attribute_offset = 0;
        for (const PipelineVertexAttribute& vertex_attribute : m_description.vertex_attributes)
        {
            // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc
            D3D11_INPUT_ELEMENT_DESC element_description = {};
            element_description.SemanticName = vertex_attribute.name.characters();
            // TODO: Actually correctly implement semantic indices!
            element_description.SemanticIndex = 0;
            element_description.Format = get_pipeline_vertex_attribute_type_format(vertex_attribute.type);
            element_description.InputSlot = 0;
            element_description.AlignedByteOffset = vertex_attribute_offset;
            element_description.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            element_description.InstanceDataStepRate = 0;

            input_element_descriptions.add(element_description);
            vertex_attribute_offset += get_pipeline_vertex_attribute_type_size(vertex_attribute.type);
        }

        // Set the stride of a vertex.
        m_vertex_stride = vertex_attribute_offset;

        Optional<ReadonlyByteSpan> vertex_shader_bytecode = m_description.shader.as<D3D11Shader>()->get_shader_module_bytecode(ShaderStage::Vertex);
        // NOTE: The provided shader doesn't have a vertex stage.
        SE_ASSERT(vertex_shader_bytecode.has_value());

        SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateInputLayout(
            input_element_descriptions.elements(),
            static_cast<UINT>(input_element_descriptions.count()),
            vertex_shader_bytecode->elements(),
            vertex_shader_bytecode->count(),
            &m_input_layout
        ));
    }

    //
    // The specification of the rasterizer state.
    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_rasterizer_desc
    //
    D3D11_RASTERIZER_DESC rasterizer_description = {};
    rasterizer_description.DepthBias = 0;
    rasterizer_description.DepthBiasClamp = 0;
    rasterizer_description.SlopeScaledDepthBias = 0;
    rasterizer_description.DepthClipEnable = false;
    rasterizer_description.ScissorEnable = false;
    rasterizer_description.MultisampleEnable = false;
    rasterizer_description.AntialiasedLineEnable = false;

    switch (m_description.fill_mode)
    {
        case PipelineFillMode::Solid: rasterizer_description.FillMode = D3D11_FILL_SOLID; break;
        case PipelineFillMode::Wireframe: rasterizer_description.FillMode = D3D11_FILL_WIREFRAME; break;

        default:
            rasterizer_description.FillMode = D3D11_FILL_SOLID;
            SE_ASSERT(false);
            break;
    }

    switch (m_description.cull_mode)
    {
        case PipelineCullMode::None: rasterizer_description.CullMode = D3D11_CULL_NONE; break;
        case PipelineCullMode::Front: rasterizer_description.CullMode = D3D11_CULL_FRONT; break;
        case PipelineCullMode::Back: rasterizer_description.CullMode = D3D11_CULL_BACK; break;

        default:
            rasterizer_description.CullMode = D3D11_CULL_NONE;
            SE_ASSERT(false);
            break;
    }

    switch (m_description.front_face_direction)
    {
        case PipelineFrontFaceDirection::Clockwise: rasterizer_description.FrontCounterClockwise = true; break;
        case PipelineFrontFaceDirection::CounterClockwise: rasterizer_description.FrontCounterClockwise = false; break;

        default:
            rasterizer_description.FrontCounterClockwise = false;
            SE_ASSERT(false);
            break;
    }

    SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateRasterizerState(&rasterizer_description, &m_rasterizer_state));
}

D3D11Pipeline::~D3D11Pipeline()
{
    SE_D3D11_RELEASE(m_rasterizer_state);
    SE_D3D11_RELEASE(m_input_layout);
}

} // namespace SE
