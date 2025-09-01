// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/VectorView.h>
#include <Runtime/Core/Memory/MemoryOperations.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderCompiler.h>

// Include the DXC compiler.
#include <Windows.h>
#include <atlbase.h>
#include <dxc/dxcapi.h>
#include <dxc/WinAdapter.h>

// Include the SPIRV-Reflect library.
#include <Runtime/Renderer/ShaderCompiler/spirv_reflect.h>

namespace SE
{

struct DXCInstance
{
    CComPtr<IDxcLibrary> Library;
    CComPtr<IDxcCompiler3> Compiler;
};
static DXCInstance* s_DXCInstance;

ShaderCompiler::ShaderCompiler(String shaderSourceCode)
    : m_SourceCode(std::move(shaderSourceCode))
{}

StringView ShaderCompiler::GetEntryPointNameForStage(ShaderStage stage)
{
    switch (stage)
    {
        case ShaderStage::Vertex:   return VIEW("VSMain");
        case ShaderStage::Fragment: return VIEW("FSMain");
    }

    SE_ASSERT_NOT_REACHED;
    return VIEW("<invalid>");
}

bool ShaderCompiler::Compile()
{
    if (!s_DXCInstance)
    {
        s_DXCInstance = new DXCInstance();

        // Initialize the DXC library.
        const HRESULT libraryCreateResult = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&s_DXCInstance->Library));
        if (FAILED(libraryCreateResult))
        {
            SE_LOG_ERROR("Failed to initialize the DXC library! (Result: %d)", libraryCreateResult);
            return false;
        }

        // Initialize the DXC compiler.
        const HRESULT compilerCreateResult = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&s_DXCInstance->Compiler));
        if (FAILED(compilerCreateResult))
        {
            SE_LOG_ERROR("Failed to initialize the DXC compiler! (Result: %d)", compilerCreateResult);
            return false;
        }
    }

    Vector<ShaderStage> existingShaderStages;

#define _SE_CHECK_IF_STAGE_IS_PRESENT(x)                                        \
    if constexpr (ShaderStage::x != ShaderStage::Unknown)                       \
    {                                                                           \
        if (m_SourceCode.Contains(GetEntryPointNameForStage(ShaderStage::x)))   \
            existingShaderStages.Add(ShaderStage::x);                           \
    }
    SE_ENUMERATE_SHADER_STAGES(_SE_CHECK_IF_STAGE_IS_PRESENT)
#undef _SE_CHECK_IF_STAGE_IS_PRESENT

    for (ShaderStage shaderStage : existingShaderStages)
    {
        Vector<const wchar_t*> compilerArguments;
        compilerArguments.EnsureCapacity(8);

        // Determine the compiler arguments.
        AddBaseCompilerArguments(compilerArguments);
        AddStageSpecificCompilerArguments(compilerArguments, shaderStage);

        // Generate the bytecode.
        Buffer bytecode = GenerateBytecodeForStage(shaderStage, compilerArguments.View());
        if (bytecode.IsEmpty())
            return false;

        // Generate the reflection data.
        Optional<ShaderReflectionData> reflectionData = GenerateReflectionData(shaderStage, bytecode);
        if (!reflectionData.HasValue())
            return false;

        // Add the stage to the compiled stages list.
        CompiledShaderStage& compiledStage = m_CompiledStages.Emplace();
        compiledStage.Stage = shaderStage;
        compiledStage.Bytecode = std::move(bytecode);
        compiledStage.ReflectionData = std::move(reflectionData.Value());
    }

    return true;
}

void ShaderCompiler::AddBaseCompilerArguments(Vector<const wchar_t*>& outArguments)
{
    // Compile to SPIR-V bytecode format (instead of the default DXIL).
    outArguments.Add(L"-spirv");
    outArguments.Add(L"-fspv-target-env=vulkan1.1");

    // Enforce DXC to decorate the generated SPIR-V bytecode with row-major matrix specifiers.
    outArguments.Add(L"-Zpr");
}

void ShaderCompiler::AddStageSpecificCompilerArguments(Vector<const wchar_t*>& outArguments, ShaderStage stage)
{
    const wchar_t* stageTarget = nullptr;
    switch (stage)
    {
        case ShaderStage::Vertex:   stageTarget = L"vs_6_1"; break;
        case ShaderStage::Fragment: stageTarget = L"ps_6_1"; break;
        default: SE_ASSERT_NOT_REACHED;
    }

    // Set the stage target.
    outArguments.Add(L"-T");
    outArguments.Add(stageTarget);

    const wchar_t* entryPointName = nullptr;
    switch (stage)
    {
        case ShaderStage::Vertex:   entryPointName = L"VSMain"; break;
        case ShaderStage::Fragment: entryPointName = L"FSMain"; break;
        default: SE_ASSERT_NOT_REACHED;
    }

    // Set the entry point.
    outArguments.Add(L"-E");
    outArguments.Add(entryPointName);

    if (stage == ShaderStage::Vertex)
    {
        // If we compile the vertex stage, flip the Y-axis. DirectX (and implicitly DXC) considers the coordinate system origin to be the top-left
        // corner, while Vulkan considers the coordinate system origin to be the bottom-left corner.
        outArguments.Add(L"-fvk-invert-y");
    }
}

Buffer ShaderCompiler::GenerateBytecodeForStage(ShaderStage stage, VectorView<const wchar_t*> compilerArguments)
{
    DxcBuffer sourceCodeBuffer = {};
    sourceCodeBuffer.Encoding = DXC_CP_ACP;
    sourceCodeBuffer.Size = m_SourceCode.ByteCountWithNullTerminator();
    sourceCodeBuffer.Ptr = m_SourceCode.Characters();

    // Run the DXC compiler.
    CComPtr<IDxcResult> stageCompilationResult;
    HRESULT compileResult = s_DXCInstance->Compiler->Compile(
        &sourceCodeBuffer,
        (LPCWSTR*)compilerArguments.Elements(), (uint32)compilerArguments.Count(),
        nullptr, IID_PPV_ARGS(&stageCompilationResult));
    if (SUCCEEDED(compileResult))
        stageCompilationResult->GetStatus(&compileResult);

    if (SUCCEEDED(compileResult))
    {
        // Read the stage bytecode.
        CComPtr<IDxcBlob> bycodeBlob;
        stageCompilationResult->GetResult(&bycodeBlob);
        Buffer bytecode;
        bytecode.SetByteCount(bycodeBlob->GetBufferSize());
        MemoryCopy(bytecode.Data(), bycodeBlob->GetBufferPointer(), bycodeBlob->GetBufferSize());
        return bytecode;
    }

    CComPtr<IDxcBlobEncoding> errorBlob;
    if (SUCCEEDED(stageCompilationResult->GetErrorBuffer(&errorBlob)) && errorBlob)
    {
        const String errorMessage = StringView::FromUTF8((const char*)errorBlob->GetBufferPointer());
        m_ErrorMessages.Add(Move(errorMessage));
    }
    else
    {
        // TODO(Traian): Include the shader stage name in the error message. Currently, this is not implemented
        // as we have no standard way to format strings in the engine.
        m_ErrorMessages.Add(VIEW("Shader compilation failed but no error messages were returned by the DXC compiler!"));
    }

    return {};
}

Optional<ShaderReflectionData> ShaderCompiler::GenerateReflectionData(ShaderStage stage, ReadonlyBufferView bytecode)
{
    SpvReflectShaderModule shaderModule = {};
    SpvReflectResult result = spvReflectCreateShaderModule(bytecode.ByteCount(), bytecode.Data(), &shaderModule);
    if (result != SPV_REFLECT_RESULT_SUCCESS)
    {
        // TODO(Traian): Include the shader stage name in the error message. Currently, this is not implemented
        // as we have no standard way to format strings in the engine.
        m_ErrorMessages.Add(VIEW("Failed to generate reflection data using SPIRV-Reflect!"));
        return {};
    }

    ShaderReflectionData reflectionData = {};

    // Read descriptor bindings.
    {
        uint32 descriptorBindingCount = 0;
        result = spvReflectEnumerateDescriptorBindings(&shaderModule, &descriptorBindingCount, nullptr);
        if (result != SPV_REFLECT_RESULT_SUCCESS)
        {
            spvReflectDestroyShaderModule(&shaderModule);
            m_ErrorMessages.Add(VIEW("Failed to read the descriptor bindings from the SPIRV-Reflect shader module!"));
            return {};
        }

        Vector<SpvReflectDescriptorBinding*> descriptorBindings;
        descriptorBindings.SetCountDefaulted(descriptorBindingCount);
        spvReflectEnumerateDescriptorBindings(&shaderModule, &descriptorBindingCount, descriptorBindings.Elements());

        for (const SpvReflectDescriptorBinding* descriptorBinding : descriptorBindings)
        {
            ShaderReflectionDescriptorBinding& binding = reflectionData.DescriptorSets[descriptorBinding->set][descriptorBinding->binding];
            binding.Name = StringView::FromUTF8(descriptorBinding->name);
            binding.ArrayCount = descriptorBinding->count;

            switch (descriptorBinding->descriptor_type)
            {
                case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                {
                    if (binding.DescriptorType == ShaderReflectionDescriptorType::Unknown)
                    {
                        binding.DescriptorType = ShaderReflectionDescriptorType::Sampler;
                    }
                    else
                    {
                        SE_ASSERT(binding.DescriptorType == ShaderReflectionDescriptorType::Image);
                        binding.DescriptorType = ShaderReflectionDescriptorType::CombinedImageSampler;
                    }
                    break;
                }

                case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                {
                    if (binding.DescriptorType == ShaderReflectionDescriptorType::Unknown)
                    {
                        binding.DescriptorType = ShaderReflectionDescriptorType::Image;
                    }
                    else
                    {
                        SE_ASSERT(binding.DescriptorType == ShaderReflectionDescriptorType::Sampler);
                        binding.DescriptorType = ShaderReflectionDescriptorType::CombinedImageSampler;
                    }
                    break;
                }

                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                {
                    SE_ASSERT(binding.DescriptorType == ShaderReflectionDescriptorType::Unknown);
                    binding.DescriptorType = ShaderReflectionDescriptorType::CombinedImageSampler;
                    break;
                }
                
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                {
                    SE_ASSERT(binding.DescriptorType == ShaderReflectionDescriptorType::Unknown);
                    binding.DescriptorType = ShaderReflectionDescriptorType::UniformBuffer;
                    break;
                }

                default:
                {
                    SE_LOG_WARN("Unknown descriptor type (generated by SPIRV-Reflect) was encountered! (%d)", descriptorBinding->descriptor_type);
                    break;
                }
            }
        }
    }

    spvReflectDestroyShaderModule(&shaderModule);
    return reflectionData;
}

bool ShaderCompiler::IsStageCompiled(ShaderStage stage) const
{
    for (const CompiledShaderStage& compiledStage : m_CompiledStages)
    {
        if (compiledStage.Stage == stage)
            return true;
    }

    return false;
}

const CompiledShaderStage& ShaderCompiler::GetCompiledStage(ShaderStage stage) const
{
    for (const CompiledShaderStage& compiledStage : m_CompiledStages)
    {
        if (compiledStage.Stage == stage)
            return compiledStage;
    }

    SE_ASSERT_NOT_REACHED;
    return m_CompiledStages.First();
}

}
