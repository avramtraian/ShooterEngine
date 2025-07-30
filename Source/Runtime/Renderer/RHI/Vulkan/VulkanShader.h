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
        VkShaderStageFlagBits Stage;
        std::string EntryPoint;
    };

public:
    VulkanShader(const ShaderInfo& info);
    virtual ~VulkanShader() override;

    NODISCARD FORCEINLINE uint32 GetModuleCount() const { return (uint32)m_Modules.size(); }
    NODISCARD FORCEINLINE const std::vector<Module>& GetModules() const { return m_Modules; }

    NODISCARD FORCEINLINE VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

private:
    std::vector<Module> m_Modules;

    std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
    VkPipelineLayout m_PipelineLayout;
};

}
