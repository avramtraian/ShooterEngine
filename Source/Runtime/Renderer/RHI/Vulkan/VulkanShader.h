// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/CommandList.h>
#include <Runtime/Renderer/RHI/Shader.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSetManager.h>

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

public:
    NODISCARD FORCEINLINE const VulkanDescriptorSetLayout& GetDescriptorSetLayout(uint32 setIndex) const
    {
        SE_ASSERT(m_DescriptorSetLayouts.contains(setIndex));
        return m_DescriptorSetLayouts.at(setIndex);
    }

    NODISCARD FORCEINLINE VulkanDescriptorSetManager& GetDescriptorSetManager() { return *m_DescriptorSetManager; }
    NODISCARD FORCEINLINE const VulkanDescriptorSetManager& GetDescriptorSetManager() const { return *m_DescriptorSetManager; }

private:
    std::vector<Module> m_Modules;

    VkPipelineLayout m_PipelineLayout;
    std::unordered_map<uint32, VulkanDescriptorSetLayout> m_DescriptorSetLayouts;
    std::unique_ptr<VulkanDescriptorSetManager> m_DescriptorSetManager;
};

}
