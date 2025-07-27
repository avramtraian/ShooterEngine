// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/Shader.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

class VulkanShader : public Shader
{
public:
    struct Module
    {
        VkShaderModule Handle { VK_NULL_HANDLE };
        VkShaderStageFlagBits VulkanStage;
        ShaderStage Stage;
    };

public:
    VulkanShader(const ShaderInfo& info);
    virtual ~VulkanShader() override;

    NODISCARD FORCEINLINE uint32 GetModuleCount() const { return (uint32)m_Modules.size(); }
    NODISCARD FORCEINLINE const std::vector<Module>& GetModules() const { return m_Modules; }

private:
    std::vector<Module> m_Modules;
};

}
