/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/RefPtr.h>
#include <Core/Containers/Span.h>
#include <Core/Containers/String.h>
#include <Core/Containers/Vector.h>

namespace SE
{

enum class ShaderStage : u8
{
    Unknown = 0,
    Vertex,
    Fragment,
};

enum class ShaderSourceType : u8
{
    Unknown = 0,
    SourceCode,
    Bytecode,
};

struct ShaderStageDescription
{
    ShaderStage stage { ShaderStage::Unknown };
    ShaderSourceType source_type { ShaderSourceType::Unknown };

    // Only used when the shader stage source type is set to `SourceCode`.
    StringView source_code;

    // Only used when the shader stage source type is set to `Bytecode`.
    ReadonlyByteSpan source_bytecode;
};

struct ShaderDescription
{
    Vector<ShaderStageDescription> stages;
    String debug_name;
};

class Shader : public RefCounted
{
public:
    NODISCARD SHOOTER_API static RefPtr<Shader> create(const ShaderDescription& info);

    Shader() = default;
    virtual ~Shader() override = default;

public:
    NODISCARD virtual bool has_stage(ShaderStage shader_stage) const = 0;
};

} // namespace SE
