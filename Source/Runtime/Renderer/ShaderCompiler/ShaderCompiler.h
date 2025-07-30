// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Memory/Buffer.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderReflectionData.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderStage.h>

#include <optional>
#include <string>
#include <vector>

namespace SE
{

struct CompiledShaderStage
{
public:
    ShaderStage          Stage { ShaderStage::Unknown };
    ShaderReflectionData ReflectionData;
    Buffer               Bytecode;
};

class ShaderCompiler
{
    SE_MAKE_NONCOPYABLE(ShaderCompiler);
    SE_MAKE_NONMOVABLE(ShaderCompiler);

public:
    SHOOTER_API ShaderCompiler(std::string shaderSourceCode);
    SHOOTER_API ~ShaderCompiler() = default;

    SHOOTER_API NODISCARD static std::string_view GetEntryPointNameForStage(ShaderStage stage);
    SHOOTER_API bool Compile();

public:
    NODISCARD FORCEINLINE bool HasErrorMessages() const { return !m_ErrorMessages.empty(); }
    NODISCARD FORCEINLINE const std::vector<std::string>& GetErrorMessages() const { return m_ErrorMessages; }
    NODISCARD FORCEINLINE const std::vector<CompiledShaderStage>& GetCompiledStages() const { return m_CompiledStages; }

    SHOOTER_API NODISCARD bool IsStageCompiled(ShaderStage stage) const;
    SHOOTER_API NODISCARD const CompiledShaderStage& GetCompiledStage(ShaderStage stage) const;

private:
    void AddBaseCompilerArguments(std::vector<const wchar_t*>& outArguments);
    void AddStageSpecificCompilerArguments(std::vector<const wchar_t*>& outArguments, ShaderStage stage);

    Buffer GenerateBytecodeForStage(ShaderStage stage, VectorView<const wchar_t*> compilerArguments);
    std::optional<ShaderReflectionData> GenerateReflectionData(ShaderStage stage, ReadonlyBufferView bytecode);

private:
    std::string m_SourceCode;
    std::vector<std::string> m_ErrorMessages;
    std::vector<CompiledShaderStage> m_CompiledStages;
};

}
