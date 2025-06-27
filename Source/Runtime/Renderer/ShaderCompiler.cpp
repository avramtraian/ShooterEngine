// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Renderer/ShaderCompiler.h>

/* NOTE(Traian): These header files are required in order to use the DXC compiler. */
#include <Windows.h>
#include <atlbase.h>
#include <dxc/dxcapi.h>
#include <dxc/WinAdapter.h>

namespace SE
{

struct ShaderCompilerData
{
    CComPtr<IDxcLibrary> Library;
    CComPtr<IDxcCompiler3> Compiler;
    CComPtr<IDxcUtils> Utilities;
};
static ShaderCompilerData* s_CompilerData;

bool ShaderCompiler::Initialize()
{
    if (s_CompilerData)
        return false;
    s_CompilerData = new ShaderCompilerData();

    /* Initialize the DXC library. */
    const HRESULT libraryCreateResult = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&s_CompilerData->Library));
    if (FAILED(libraryCreateResult))
    {
        SE_LOG_ERROR("Failed to initialize the DXC library! (Result: %d)", libraryCreateResult);
        return false;
    }

    /* Initialize the DXC compiler. */
    const HRESULT compilerCreateResult = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&s_CompilerData->Compiler));
    if (FAILED(compilerCreateResult))
    {
        SE_LOG_ERROR("Failed to initialize the DXC compiler! (Result: %d)", compilerCreateResult);
        return false;
    }

    /* Initialize the DXC utilities. */
    const HRESULT utilitiesCreateResult = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&s_CompilerData->Utilities));
    if (FAILED(utilitiesCreateResult))
    {
        SE_LOG_ERROR("Failed to initialize the DXC utilities! (Result: %d)", utilitiesCreateResult);
        return false;
    }

    return true;
}

void ShaderCompiler::Shutdown()
{
    if (!s_CompilerData)
        return;

    s_CompilerData->Utilities.Release();
    s_CompilerData->Compiler.Release();
    s_CompilerData->Library.Release();

    delete s_CompilerData;
    s_CompilerData = nullptr;
}

struct ShaderEntrypointAndTarget
{
    std::wstring EntryPoint;
    std::wstring Target;
};

static ShaderEntrypointAndTarget GetShaderEntryPointAndTargetForStage(ShaderStage stage)
{
    switch (stage)
    {
        case ShaderStage::Vertex:   return { L"VSMain", L"vs_6_1" };
        case ShaderStage::Fragment: return { L"FSMain", L"ps_6_1" };
    }

    SE_ASSERT_NOT_REACHED;
    return {};
}

ShaderCompilationResult ShaderCompiler::Compile(ShaderSourceLanguage language, ShaderBytecodeType outputBytecodeType, const std::string& sourceCode)
{
    /* List of all shader stages that are present in the shader source code. */
    std::vector<ShaderStage> shaderStages;

#define _SE_CHECK_STAGE(x) if (CheckIfShaderStageIsPresent(ShaderStage::x, sourceCode)) { shaderStages.push_back(ShaderStage::x); }
    SE_ENUMERATE_SHADER_STAGES(_SE_CHECK_STAGE)
#undef _SE_CHECK_STAGE

    ShaderCompilationResult compilationResult = {};
    compilationResult.SetBytecodeType(outputBytecodeType);

    for (ShaderStage stage : shaderStages)
    {
        std::vector<const wchar_t*> arguments;

        /* Set the shader entry point and target for the current stage. */
        const ShaderEntrypointAndTarget entryPointAndTarget = GetShaderEntryPointAndTargetForStage(stage);
        arguments.push_back(L"-E"); arguments.push_back(entryPointAndTarget.EntryPoint.c_str());
        arguments.push_back(L"-T"); arguments.push_back(entryPointAndTarget.Target.c_str());

        /* Set the bytecode output type. */
        if (outputBytecodeType == ShaderBytecodeType::SPIRV) { arguments.push_back(L"-spirv"); }
        if (outputBytecodeType == ShaderBytecodeType::DXIL)  { /* No additional arguments are required. */ }

        /* Create the source code buffer. */
        DxcBuffer sourceCodeBuffer = {};
        sourceCodeBuffer.Encoding = DXC_CP_ACP;
        sourceCodeBuffer.Size = sourceCode.size();
        sourceCodeBuffer.Ptr = sourceCode.c_str();

        CComPtr<IDxcResult> stageCompilationResult;
        HRESULT compileResult = s_CompilerData->Compiler->Compile(
            &sourceCodeBuffer,
            arguments.data(), (uint32)arguments.size(),
            nullptr, IID_PPV_ARGS(&stageCompilationResult)
        );
        if (SUCCEEDED(compileResult))
            stageCompilationResult->GetStatus(&compileResult);

        if (SUCCEEDED(compileResult))
        {
            CComPtr<IDxcBlob> bycodeBlob;
            stageCompilationResult->GetResult(&bycodeBlob);

            std::vector<uint8> bytecode;
            bytecode.resize(bycodeBlob->GetBufferSize());
            MemoryCopy(bytecode.data(), bycodeBlob->GetBufferPointer(), bytecode.size());

            compilationResult.AddCompiledStage(ShaderStageInfo()
                .SetStage(stage)
                .SetBytecode(std::move(bytecode))
            );
        }
        else
        {
            CComPtr<IDxcBlobEncoding> errorBlob;
            if (SUCCEEDED(stageCompilationResult->GetErrorBuffer(&errorBlob)) && errorBlob)
            {
                /* Append the compilation error to the error stream. */
                compilationResult.AddToErrorStream((const char*)errorBlob->GetBufferPointer());

                SE_LOG_ERROR(
                    "Shader compilation for stage '%s' failed with the following error message:\n%s",
                    ShaderStageToString(stage).c_str(), (const char*)errorBlob->GetBufferPointer()
                );
            }
        }
    }

    return compilationResult;
}

bool ShaderCompiler::CheckIfShaderStageIsPresent(ShaderStage stage, const std::string& sourceCode)
{
    std::string pragmaTokenName;

    switch (stage)
    {
        case ShaderStage::Vertex:   pragmaTokenName = "vertex"; break;
        case ShaderStage::Fragment: pragmaTokenName = "fragment"; break;
    }

    /* Invalid shader stage. */
    if (pragmaTokenName.empty())
        return false;

    const std::string pragmaExpression = std::string("#pragma shader_stage(") + pragmaTokenName + std::string(")");
    const size_t findPosition = sourceCode.find(pragmaExpression);
    return (findPosition != std::string::npos);
}

}
