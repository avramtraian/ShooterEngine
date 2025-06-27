// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/ShaderStage.h>

#include <string>
#include <vector>

namespace SE
{

struct ShaderInfo
{
public:
    std::string DebugName;
    std::vector<ShaderStageInfo> Stages;

public:
    inline ShaderInfo& SetDebugName (std::string_view debugName)          { DebugName = debugName;      return *this; }
    inline ShaderInfo& SetStages    (std::vector<ShaderStageInfo> stages) { Stages = std::move(stages); return *this; }
    inline ShaderInfo& AddStage     (const ShaderStageInfo& stage)        { Stages.push_back(stage);    return *this; }
};

class Shader
{
    SE_MAKE_RENDERER_RHI_INTERFACE(Shader);
};

}
