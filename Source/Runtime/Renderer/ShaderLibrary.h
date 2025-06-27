// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Renderer/RHI/Shader.h>

#include <filesystem>
#include <memory>
#include <string_view>

namespace SE
{

class ShaderLibrary
{
    SE_MAKE_NAMESPACE_CLASS(ShaderLibrary);

public:
    static bool Initialize();
    static void Shutdown();

    /* Reads the shader source code from the provided file, compiles it, and creates an RHI shader object.
     * This object can later be retrieved using the 'Get' API. */
    SHOOTER_API static bool LoadFromFile(std::string shaderName, const std::filesystem::path& sourceFilepath);
    
    /* Returns the RHI shader object associated with the given name. If no shader in the library is associated
     * with the provided name, an invalid shared pointer (nullptr) will be returned. */
    SHOOTER_API static std::shared_ptr<Shader> Get(const std::string& shaderName);
};

}
