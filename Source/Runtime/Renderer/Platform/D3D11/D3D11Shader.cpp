/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/FileSystem/FileSystem.h>
#include <Core/Log.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>
#include <Renderer/Platform/D3D11/D3D11Shader.h>

#include <d3dcompiler.h>

namespace SE
{

static LPCSTR get_shader_stage_entry_point(ShaderStageType stage)
{
    switch (stage)
    {
        case ShaderStageType::Vertex: return "vertex_main";
        case ShaderStageType::Fragment: return "fragment_main";
    }

    SE_LOG_TAG_ERROR("D3D11"sv, "Invalid ShaderStageType!"sv);
    SE_ASSERT(false);
    return nullptr;
}

static LPCSTR get_shader_stage_target(ShaderStageType stage)
{
    switch (stage)
    {
        case ShaderStageType::Vertex: return "vs_5_0";
        case ShaderStageType::Fragment: return "ps_5_0";
    }

    SE_LOG_TAG_ERROR("D3D11"sv, "Invalid ShaderStageType!"sv);
    SE_ASSERT(false);
    return nullptr;
}

D3D11Shader::D3D11Shader(const ShaderInfo& info)
{
    for (const ShaderStage& stage_info : info.stages)
    {
        Stage& stage = m_stages.emplace();
        stage.type = stage_info.type;

        FileReader shader_source_reader;
        Buffer shader_source;
        SE_CHECK_FILE_ERROR(shader_source_reader.open(stage_info.filepath));
        SE_CHECK_FILE_ERROR(shader_source_reader.read_entire(shader_source));
        shader_source_reader.close();

        ID3DBlob* error_messages_blob = nullptr;
        const HRESULT compilation_result = D3DCompile(
            shader_source.bytes(),
            shader_source.byte_count(),
            nullptr,
            nullptr,
            nullptr,
            get_shader_stage_entry_point(stage_info.type),
            get_shader_stage_target(stage_info.type),
            0,
            0,
            &stage.byte_code,
            &error_messages_blob
        );
        shader_source.release();

        if (FAILED(compilation_result))
        {
            StringView error_message = "Unknown"sv;

            if (error_messages_blob)
            {
                error_message = StringView::create_from_utf8((const char*)(error_messages_blob->GetBufferPointer()), error_messages_blob->GetBufferSize());
            }

            SE_LOG_TAG_ERROR("D3D11"sv, "Shader '{}' compilation failed:\n{}"sv, stage_info.filepath, error_message);
            SE_ASSERT(false);
        }

        if (error_messages_blob)
            error_messages_blob->Release();

        switch (stage_info.type)
        {
            case ShaderStageType::Vertex:
            {
                SE_D3D11_CHECK(D3D11Renderer::get_device()->CreateVertexShader(
                    stage.byte_code->GetBufferPointer(), stage.byte_code->GetBufferSize(), nullptr, &stage.shader.vertex
                ));
                break;
            }

            case ShaderStageType::Fragment:
            {
                SE_D3D11_CHECK(D3D11Renderer::get_device()->CreatePixelShader(
                    stage.byte_code->GetBufferPointer(), stage.byte_code->GetBufferSize(), nullptr, &stage.shader.pixel
                ));
                break;
            }
        }
    }
}

D3D11Shader::~D3D11Shader()
{
    for (Stage& stage : m_stages)
    {
        switch (stage.type)
        {
            case ShaderStageType::Vertex: stage.shader.vertex->Release(); break;

            case ShaderStageType::Fragment: stage.shader.pixel->Release(); break;
        }

        if (stage.byte_code)
        {
            stage.byte_code->Release();
            stage.byte_code = nullptr;
        }
    }
}

Optional<ReadonlyByteSpan> D3D11Shader::get_bytecode(ShaderStageType stage_type /*= ShaderStageType::Vertex*/) const
{
    for (const Stage& stage : m_stages)
    {
        if (stage.type == stage_type)
            return ReadonlyByteSpan((ReadonlyBytes)(stage.byte_code->GetBufferPointer()), stage.byte_code->GetBufferSize());
    }
    return {};
}

ID3D11DeviceChild* D3D11Shader::get_handle(ShaderStageType stage_type) const
{
    for (const Stage& stage : m_stages)
    {
        if (stage.type == stage_type)
            return static_cast<ID3D11DeviceChild*>(stage.shader.vertex);
    }
    return nullptr;
}

} // namespace SE
