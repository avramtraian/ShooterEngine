// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/OwnPtr.h>
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
        String EntryPoint;
    };

public:
    VulkanShader(const ShaderInfo& info);
    virtual ~VulkanShader() override;

    NODISCARD ALWAYS_INLINE uint32 GetModuleCount() const { return (uint32)m_Modules.Count(); }
    NODISCARD ALWAYS_INLINE const Vector<Module>& GetModules() const { return m_Modules; }

    NODISCARD ALWAYS_INLINE VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

public:
    NODISCARD ALWAYS_INLINE const HashMap<uint32, VulkanDescriptorSetLayout>& GetDescriptorSetLayouts() const { return m_DescriptorSetLayouts; }
    NODISCARD ALWAYS_INLINE VulkanDescriptorSetManager& GetDescriptorSetManager() { return *m_DescriptorSetManager; }
    NODISCARD ALWAYS_INLINE const VulkanDescriptorSetManager& GetDescriptorSetManager() const { return *m_DescriptorSetManager; }

    NODISCARD ALWAYS_INLINE const VulkanDescriptorSetLayout& GetDescriptorSetLayout(uint32 setIndex) const
    {
        SE_ASSERT(m_DescriptorSetLayouts.Contains(setIndex));
        return m_DescriptorSetLayouts.At(setIndex);
    }

private:
    Vector<Module> m_Modules;

    VkPipelineLayout m_PipelineLayout;
    HashMap<uint32, VulkanDescriptorSetLayout> m_DescriptorSetLayouts;
    OwnPtr<VulkanDescriptorSetManager> m_DescriptorSetManager;
};

}
