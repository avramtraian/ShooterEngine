// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Containers/String/StringView.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Memory/Buffer.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderReflectionData.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderStage.h>

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
    SHOOTER_API ShaderCompiler(String shaderSourceCode);
    SHOOTER_API ~ShaderCompiler() = default;

    NODISCARD SHOOTER_API static StringView GetEntryPointNameForStage(ShaderStage stage);
    SHOOTER_API bool Compile();

public:
    NODISCARD FORCEINLINE bool HasErrorMessages() const { return m_ErrorMessages.HasElements(); }
    NODISCARD FORCEINLINE const Vector<String>& GetErrorMessages() const { return m_ErrorMessages; }
    NODISCARD FORCEINLINE const Vector<CompiledShaderStage>& GetCompiledStages() const { return m_CompiledStages; }

    NODISCARD SHOOTER_API bool IsStageCompiled(ShaderStage stage) const;
    NODISCARD SHOOTER_API const CompiledShaderStage& GetCompiledStage(ShaderStage stage) const;

private:
    void AddBaseCompilerArguments(Vector<const wchar_t*>& outArguments);
    void AddStageSpecificCompilerArguments(Vector<const wchar_t*>& outArguments, ShaderStage stage);

    Buffer GenerateBytecodeForStage(ShaderStage stage, VectorView<const wchar_t*> compilerArguments);
    Optional<ShaderReflectionData> GenerateReflectionData(ShaderStage stage, ReadonlyBufferView bytecode);

private:
    String                      m_SourceCode;
    Vector<String>              m_ErrorMessages;
    Vector<CompiledShaderStage> m_CompiledStages;
};

} // namespace SE
