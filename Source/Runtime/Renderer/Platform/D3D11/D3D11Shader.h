/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Optional.h>
#include <Core/Memory/Buffer.h>
#include <Renderer/Platform/D3D11/D3D11Headers.h>
#include <Renderer/Shader.h>

namespace SE
{

class D3D11Shader final : public Shader
{
public:
    struct Stage
    {
        union
        {
            ID3D11VertexShader* vertex;
            ID3D11PixelShader* pixel;
        } shader;
        ShaderStageType type;
        ID3DBlob* byte_code;
    };

public:
    D3D11Shader(const ShaderInfo& info);
    virtual ~D3D11Shader() override;

    // Returns an empty optional if the given shader stage is not present.
    Optional<ReadonlyByteSpan> get_bytecode(ShaderStageType stage_type = ShaderStageType::Vertex) const;

    // Returns nullptr if the given shader stage is not present.
    ID3D11DeviceChild* get_handle(ShaderStageType stage_type) const;

private:
    Vector<Stage> m_stages;
};

} // namespace SE
