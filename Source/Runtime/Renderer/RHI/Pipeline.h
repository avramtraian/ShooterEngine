// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Shader.h>

#include <memory>
#include <string>
#include <string_view>

namespace SE
{

struct PipelineInfo
{
public:
    std::string DebugName;
    std::shared_ptr<Shader> Shader;

public:
    inline PipelineInfo& SetDebugName(std::string_view debugName) { DebugName = debugName; return *this; }
    inline PipelineInfo& SetShader(const std::shared_ptr<class Shader> shader) { Shader = shader; }
};

class Pipeline
{
    SE_MAKE_RENDERER_RHI_INTERFACE(Pipeline);

public:
};

}
