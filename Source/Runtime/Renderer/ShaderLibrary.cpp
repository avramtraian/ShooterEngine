// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Renderer/RHI/RenderingDriver.h>
#include <Runtime/Renderer/RHI/Shader.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderCompiler.h>
#include <Runtime/Renderer/ShaderLibrary.h>

#include <fstream>

namespace SE
{

struct ShaderLibraryData
{
    HashMap<String, RefPtr<Shader>> LoadedShaders;
};
static ShaderLibraryData* s_LibraryData;

bool ShaderLibrary::Initialize()
{
    if (s_LibraryData)
        return false;
    s_LibraryData = new ShaderLibraryData();

    return true;
}

void ShaderLibrary::Shutdown()
{
    if (!s_LibraryData)
        return;

    /* NOTE(Traian): This will also release all references held by the shader library. */
    s_LibraryData->LoadedShaders.ClearAndShrink();

    delete s_LibraryData;
    s_LibraryData = nullptr;
}

bool ShaderLibrary::LoadFromFile(String shaderName, const std::filesystem::path& sourceFilepath)
{
    if (!s_LibraryData)
        return false;

    // There already exists a shader associated with the given name.
    if (s_LibraryData->LoadedShaders.Contains(shaderName))
        return false;

    // Read the source code from the provided source file.
    std::ifstream fileInputStream(sourceFilepath);
    if (!fileInputStream.is_open())
    {
        SE_LOG_WARN(
            "Failed to open file '%s' required for loading shader '%s'!",
            sourceFilepath.c_str(),
            shaderName.Characters()
        );
        return false;
    }
    std::stringstream fileData;
    fileData << fileInputStream.rdbuf();
    const String shaderSourceCode = StringView::FromUTF8(fileData.str().c_str());

    // Compile the shader from source code.
    ShaderCompiler compiler = ShaderCompiler(shaderSourceCode);
    if (!compiler.Compile())
    {
        SE_LOG_WARN("Shader '%s' compilation has failed with the following error messages:", shaderName.Characters());
        for (MAYBE_UNUSED const String& errorMessage : compiler.GetErrorMessages())
        {
            SE_LOG_WARN("  %s", errorMessage.Characters());
        }

        return false;
    }

    // Create the RHI shader object information structure which contains the compiled stages.
    ShaderInfo shaderInfo = {};
    for (const CompiledShaderStage& compiledStage : compiler.GetCompiledStages())
    {
        shaderInfo.AddStage(ShaderStageInfo()
            .SetStage(compiledStage.Stage)
            .SetBytecode(compiledStage.Bytecode.ToVectorView())
            .SetReflectionData(compiledStage.ReflectionData));
    }

    // Create the RHI shader object.
    RefPtr<Shader> loadedShader = g_RenderingDriver->CreateShader(shaderInfo);
    if (!loadedShader.IsValid())
    {
        SE_LOG_WARN("Shader '%s' RHI object wasn't created successfully.", shaderName.Characters());
        return false;
    }

    // Insert the loaded shader in the table.
    s_LibraryData->LoadedShaders.Add(Move(shaderName), loadedShader );
    return true;
}

RefPtr<Shader> ShaderLibrary::Get(const String& shaderName)
{
    if (!s_LibraryData)
        return nullptr;

    auto shader = s_LibraryData->LoadedShaders.GetIfExists(shaderName);
    if (shader.HasValue())
        return shader.Value();

    // NOTE(Traian): There is no shader with the specified name.
    return {};
}

}
