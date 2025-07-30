// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanShader.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderCompiler.h>

namespace SE
{

VulkanShader::VulkanShader(const ShaderInfo& info)
    : m_PipelineLayout(VK_NULL_HANDLE)
{
    int32 maxSetIndex = -1;

    // Create the shaders module.
    for (const ShaderStageInfo& shaderStage : info.Stages)
    {
        // Determine the max set index used by the shader.
        for (const auto& [setIndex, descriptorSet] : shaderStage.ReflectionData.DescriptorSets)
        {
            if ((int32)setIndex > maxSetIndex)
                maxSetIndex = setIndex;
        }

        // Create the shader module object.
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = shaderStage.Bytecode.Count();
        shaderModuleCreateInfo.pCode = (const uint32*)shaderStage.Bytecode.Elements();
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        SE_VULKAN_CHECK(vkCreateShaderModule(g_VulkanDriver->GetDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule));

        Module& module = m_Modules.emplace_back();
        module.Handle = shaderModule;
        module.EntryPoint = ShaderCompiler::GetEntryPointNameForStage(shaderStage.Stage);

        switch (shaderStage.Stage)
        {
            case ShaderStage::Vertex:   module.Stage = VK_SHADER_STAGE_VERTEX_BIT;   break;
            case ShaderStage::Fragment: module.Stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
            default: SE_ASSERT_NOT_REACHED;
        }
    }

    std::vector<std::unordered_map<uint32, VkDescriptorSetLayoutBinding>> bindingsPerSet;
    bindingsPerSet.resize(maxSetIndex + 1);

    for (const ShaderStageInfo& shaderStage : info.Stages)
    {
        for (const auto& [setIndex, descriptorSet] : shaderStage.ReflectionData.DescriptorSets)
        {
            for (const auto& [bindingIndex, descriptorBinding] : descriptorSet.Bindings)
            {
                const bool bindingAlreadyExists = bindingsPerSet[setIndex].contains(bindingIndex);
                VkDescriptorSetLayoutBinding& descriptorSetLayoutBinding = bindingsPerSet[setIndex][bindingIndex];
                descriptorSetLayoutBinding.binding = bindingIndex;
                descriptorSetLayoutBinding.descriptorCount = descriptorBinding.ArrayCount;
                descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

                switch (descriptorBinding.DescriptorType)
                {
                    // Supported descriptor types.
                    case ShaderReflectionDescriptorType::CombinedImageSampler: descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
                    
                    // Unsupported descriptor types.
                    case ShaderReflectionDescriptorType::Image:
                    case ShaderReflectionDescriptorType::Sampler:
                    {
                        // NOTE(Traian): Vulkan "only" supports ShaderReflectionDescriptorType::CombinedImageSampler!
                        SE_LOG_ERROR("Vulkan doesn't support the following descriptor type: %d", descriptorBinding.DescriptorType);
                        SE_ASSERT_NOT_REACHED;
                    }

                    default: SE_ASSERT_NOT_REACHED;
                }

                VkShaderStageFlags currentStageFlags = 0;
                switch (shaderStage.Stage)
                {
                    case ShaderStage::Vertex:   currentStageFlags = VK_SHADER_STAGE_VERTEX_BIT; break;
                    case ShaderStage::Fragment: currentStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; break;
                    default: SE_ASSERT_NOT_REACHED;
                }
                descriptorSetLayoutBinding.stageFlags = bindingAlreadyExists
                    ? descriptorSetLayoutBinding.stageFlags | currentStageFlags
                    : currentStageFlags;
            }
        }
    }

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutbindings;
    for (int32 setIndex = 0; setIndex <= maxSetIndex; ++setIndex)
    {
        if (bindingsPerSet[setIndex].empty())
        {
            // The indices corresponding to each descriptor set layout are implicitly determined by the order in which elements
            // appear in the 'VkPipelineLayoutCreateInfo::pSetLayouts' array. If a descriptor set index is not used by the shader,
            // it is acceptable for the corresponding set layout to be VK_NULL_HANDLE.
            // 
            // For example, if a shader uses descriptor sets 0, 1 and 3, in order to correctly implement this "gap" feature we must
            // set the descriptor set layout corresponding to index 2 to VK_NULL_HANDLE. */
            m_DescriptorSetLayouts.push_back(VK_NULL_HANDLE);
        }

        descriptorSetLayoutbindings.clear();
        descriptorSetLayoutbindings.reserve(bindingsPerSet[setIndex].size());
        for (const auto& [bindingIndex, binding] : bindingsPerSet[setIndex])
            descriptorSetLayoutbindings.push_back(binding);

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = (uint32)descriptorSetLayoutbindings.size();
        descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutbindings.data();

        VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
        SE_VULKAN_CHECK(vkCreateDescriptorSetLayout(g_VulkanDriver->GetDevice(), &descriptorSetLayoutCreateInfo, nullptr, &setLayout));
        m_DescriptorSetLayouts.push_back(setLayout);
    }

    // Create the pipeline layout.
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = (uint32)m_DescriptorSetLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = m_DescriptorSetLayouts.data();

    SE_VULKAN_CHECK(vkCreatePipelineLayout(g_VulkanDriver->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));
}

VulkanShader::~VulkanShader()
{
    // Destroy the descriptor set layouts.
    for (VkDescriptorSetLayout setLayout : m_DescriptorSetLayouts)
        vkDestroyDescriptorSetLayout(g_VulkanDriver->GetDevice(), setLayout, nullptr);
    m_DescriptorSetLayouts.clear();

    // Destroy the pipeline layout.
    vkDestroyPipelineLayout(g_VulkanDriver->GetDevice(), m_PipelineLayout, nullptr);
    m_PipelineLayout = VK_NULL_HANDLE;

    // Destroy the shader modules.
    for (Module& module : m_Modules)
        vkDestroyShaderModule(g_VulkanDriver->GetDevice(), module.Handle, nullptr);
    m_Modules.clear();
}

}
