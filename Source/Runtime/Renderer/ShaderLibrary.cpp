// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Log.h>
#include <Runtime/Renderer/RHI/RenderingDriver.h>
#include <Runtime/Renderer/RHI/Shader.h>
#include <Runtime/Renderer/ShaderCompiler.h>
#include <Runtime/Renderer/ShaderLibrary.h>

#include <fstream>
#include <unordered_map>

namespace SE
{

struct ShaderLibraryData
{
    std::unordered_map<std::string, std::shared_ptr<Shader>> LoadedShaders;
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
    s_LibraryData->LoadedShaders.clear();

    delete s_LibraryData;
    s_LibraryData = nullptr;
}

bool ShaderLibrary::LoadFromFile(std::string shaderName, const std::filesystem::path& sourceFilepath)
{
    if (!s_LibraryData)
        return false;

    /* There already exists a shader associated with the given name. */
    if (s_LibraryData->LoadedShaders.contains(shaderName))
        return false;

    std::ifstream fileInputStream(sourceFilepath);
    if (!fileInputStream.is_open())
    {
        SE_LOG_WARN(
            "Failed to open file '%s' required for loading shader '%s'!",
            sourceFilepath.c_str(),
            shaderName.c_str()
        );
        return false;
    }

    std::stringstream fileData;
    fileData << fileInputStream.rdbuf();

    /* Compile the shader. */
    const ShaderCompilationResult compilationResult = ShaderCompiler::Compile(
        ShaderSourceLanguage::HLSL,
        ShaderBytecodeType::SPIRV,
        fileData.str()
    );

    if (!compilationResult.ErrorStream.empty())
    {
        /* NOTE(Traian): The shader compilation failed. The compile function already logs the
         * compilation errors/warnings, so we don't have to do it again. */
        return false;
    }

    /* Create the RHI shader. */
    std::shared_ptr<Shader> loadedShader = g_RenderingDriver->CreateShader(ShaderInfo()
        .SetStages(compilationResult.CompiledStages)
    );
    if (!loadedShader)
    {
        SE_LOG_WARN("Shader '%s' wasn't created successfully.", shaderName);
        return false;
    }

    /* Insert the loaded shader in the table. */
    s_LibraryData->LoadedShaders.insert({ std::move(shaderName), loadedShader });
    return true;
}

std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& shaderName)
{
    if (!s_LibraryData)
        return nullptr;

    if (!s_LibraryData->LoadedShaders.contains(shaderName))
        return nullptr;

    return s_LibraryData->LoadedShaders.at(shaderName);
}

}
