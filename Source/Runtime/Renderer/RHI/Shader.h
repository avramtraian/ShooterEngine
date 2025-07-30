// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderReflectionData.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderStage.h>

#include <string>
#include <vector>

namespace SE
{

struct ShaderStageInfo
{
public:
    ShaderStage Stage { ShaderStage::Unknown };
    std::vector<uint8> Bytecode;
    ShaderReflectionData ReflectionData;

public:
    inline ShaderStageInfo& SetStage          (ShaderStage stage)                   { Stage = stage;                              return *this; }
    inline ShaderStageInfo& SetBytecode       (std::vector<uint8> bytecode)         { Bytecode = std::move(bytecode);             return *this; }
    inline ShaderStageInfo& SetReflectionData (ShaderReflectionData reflectionData) { ReflectionData = std::move(reflectionData); return *this; }
};

struct ShaderInfo
{
public:
    std::vector<ShaderStageInfo> Stages;

public:
    inline ShaderInfo& AddStage (ShaderStageInfo stage) { Stages.push_back(std::move(stage)); return *this; }
};

class Shader
{
    SE_MAKE_RENDERER_RHI_INTERFACE(Shader);
};

}
