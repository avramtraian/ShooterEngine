// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Containers/String/StringView.h>
#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

#include <cstring>

namespace SE
{

enum class ShaderStage : uint8
{
#define SE_ENUMERATE_SHADER_STAGES(x) x(Unknown) x(Vertex) x(Fragment)

#define _SE_ENUM_MEMBER(x)            x,
    SE_ENUMERATE_SHADER_STAGES(_SE_ENUM_MEMBER)
#undef _SE_ENUM_MEMBER
};

NODISCARD ALWAYS_INLINE const char* ShaderStageToRawString(ShaderStage stage)
{
    switch (stage)
    {
#define _SE_SWITCH_CASE(x) \
    case ShaderStage::x: return #x;
        SE_ENUMERATE_SHADER_STAGES(_SE_SWITCH_CASE)
#undef _SE_SWITCH_CASE
    }

    SE_ASSERT_NOT_REACHED;
    return nullptr;
}

NODISCARD ALWAYS_INLINE StringView ShaderStageToStringView(ShaderStage stage)
{
    switch (stage)
    {
#define _SE_SWITCH_CASE(x) \
    case ShaderStage::x: return VIEW(#x);
        SE_ENUMERATE_SHADER_STAGES(_SE_SWITCH_CASE)
#undef _SE_SWITCH_CASE
    }

    SE_ASSERT_NOT_REACHED;
    return {};
}

NODISCARD ALWAYS_INLINE String ShaderStageToString(ShaderStage stage)
{
    switch (stage)
    {
#define _SE_SWITCH_CASE(x) \
    case ShaderStage::x: return VIEW(#x);
        SE_ENUMERATE_SHADER_STAGES(_SE_SWITCH_CASE)
#undef _SE_SWITCH_CASE
    }

    SE_ASSERT_NOT_REACHED;
    return {};
}

NODISCARD ALWAYS_INLINE ShaderStage ShaderStageFromRawString(const char* stageString)
{
#define _SE_IF_CASE(x)                     \
    if (std::strcmp(stageString, #x) == 0) \
    {                                      \
        return ShaderStage::x;             \
    }
    SE_ENUMERATE_SHADER_STAGES(_SE_IF_CASE)
#undef _SE_IF_CASE
    SE_ASSERT_NOT_REACHED;
}

NODISCARD ALWAYS_INLINE ShaderStage ShaderStageFromStringView(StringView stageString)
{
#define _SE_IF_CASE(x)           \
    if (stageString == VIEW(#x)) \
    {                            \
        return ShaderStage::x;   \
    }
    SE_ENUMERATE_SHADER_STAGES(_SE_IF_CASE)
#undef _SE_IF_CASE
    SE_ASSERT_NOT_REACHED;
}

NODISCARD ALWAYS_INLINE ShaderStage ShaderStageFromString(const String& stageString)
{
#define _SE_IF_CASE(x)           \
    if (stageString == VIEW(#x)) \
    {                            \
        return ShaderStage::x;   \
    }
    SE_ENUMERATE_SHADER_STAGES(_SE_IF_CASE)
#undef _SE_IF_CASE
    SE_ASSERT_NOT_REACHED;
}

} // namespace SE
