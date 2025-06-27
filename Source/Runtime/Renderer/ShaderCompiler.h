// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Renderer/ShaderStage.h>

#include <string>
#include <vector>

namespace SE
{

enum class ShaderSourceLanguage : uint8
{
    Uknown = 0,
    HLSL,
    GLSL,
};

enum class ShaderBytecodeType : uint8
{
    Unknown = 0,
    SPIRV,
    DXIL,
};

struct ShaderCompilationResult
{
public:
    ShaderBytecodeType BytecodeType { ShaderBytecodeType::Unknown };
    std::string ErrorStream;
    std::vector<ShaderStageInfo> CompiledStages;

public:
    inline ShaderCompilationResult& SetBytecodeType  (ShaderBytecodeType bytecodeType)      { BytecodeType = bytecodeType;             return *this; }
    inline ShaderCompilationResult& AddToErrorStream (std::string_view errorMessage)        { ErrorStream.assign(errorMessage);        return *this; }
    inline ShaderCompilationResult& AddCompiledStage (const ShaderStageInfo& compiledStage) { CompiledStages.push_back(compiledStage); return *this; }
};

class ShaderCompiler
{
    SE_MAKE_NAMESPACE_CLASS(ShaderCompiler);

public:
    SHOOTER_API static bool Initialize();
    SHOOTER_API static void Shutdown();

    SHOOTER_API static ShaderCompilationResult Compile(ShaderSourceLanguage language, ShaderBytecodeType outputBytecodeType, const std::string& sourceCode);

private:
    static bool CheckIfShaderStageIsPresent(ShaderStage stage, const std::string& sourceCode);
};

}
