// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

#include <string>
#include <string_view>
#include <vector>

namespace SE
{

enum class ShaderStage : uint8
{
#define SE_ENUMERATE_SHADER_STAGES(x) \
    x(Unknown)                        \
    x(Vertex)                         \
    x(Fragment)                       \
    x(MaxEnumCount)

#define _SE_ENUM_MEMBER(x) x,
    SE_ENUMERATE_SHADER_STAGES(_SE_ENUM_MEMBER)
#undef _SE_ENUM_MEMBER
};

NODISCARD inline std::string ShaderStageToString(ShaderStage stage)
{
    switch (stage)
    {
#define _SE_SWITCH_CASE(x) case ShaderStage::x: return #x;
        SE_ENUMERATE_SHADER_STAGES(_SE_SWITCH_CASE)
#undef _SE_SWITCH_CASE
    }

    SE_ASSERT_NOT_REACHED;
    return "<unstringyfiable>";
}

NODISCARD inline ShaderStage ShaderStageFromString(std::string_view stageString)
{
#define _SE_IF_STATEMENT(x) if (stageString == #x) return ShaderStage::x;
    SE_ENUMERATE_SHADER_STAGES(_SE_IF_STATEMENT)
#undef _SE_IF_STATEMENT

    SE_ASSERT_NOT_REACHED;
    return ShaderStage::Unknown;
}

struct ShaderStageInfo
{
public:
    ShaderStage Stage { ShaderStage::Unknown };
    std::vector<uint8> Bytecode;

public:
    inline ShaderStageInfo& SetStage    (ShaderStage stage)           { Stage = stage;                  return *this; }
    inline ShaderStageInfo& SetBytecode (std::vector<uint8> bytecode) { Bytecode = std::move(bytecode); return *this; }
};

}
