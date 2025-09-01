// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanShader.h>
#include <Runtime/Renderer/ShaderCompiler/ShaderCompiler.h>

namespace SE
{

VulkanShader::VulkanShader(const ShaderInfo& info)
    : m_PipelineLayout(VK_NULL_HANDLE)
{
    if (info.Stages.IsEmpty())
    {
        SE_LOG_ERROR("Trying to create a [Vulkan] shader that has no stages!");
        SE_ASSERT_NOT_REACHED;
        return;
    }

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

        Module& module = m_Modules.Emplace();
        module.Handle = shaderModule;
        module.EntryPoint = ShaderCompiler::GetEntryPointNameForStage(shaderStage.Stage);

        switch (shaderStage.Stage)
        {
            case ShaderStage::Vertex:   module.Stage = VK_SHADER_STAGE_VERTEX_BIT;   break;
            case ShaderStage::Fragment: module.Stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
            default: SE_ASSERT_NOT_REACHED;
        }
    }

    Vector<HashMap<uint32, VkDescriptorSetLayoutBinding>> bindingsPerSet;
    bindingsPerSet.SetCountDefaulted(maxSetIndex + 1);

    for (const ShaderStageInfo& shaderStage : info.Stages)
    {
        for (const auto& [setIndex, descriptorSet] : shaderStage.ReflectionData.DescriptorSets)
        {
            for (const auto& [bindingIndex, descriptorBinding] : descriptorSet.Bindings)
            {
                const bool bindingAlreadyExists = bindingsPerSet[setIndex].Contains(bindingIndex);
                VkDescriptorSetLayoutBinding& descriptorSetLayoutBinding = bindingsPerSet[setIndex][bindingIndex];
                descriptorSetLayoutBinding.binding = bindingIndex;
                descriptorSetLayoutBinding.descriptorCount = descriptorBinding.ArrayCount;
                descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

                switch (descriptorBinding.DescriptorType)
                {
                    // Supported descriptor types.
                    case ShaderReflectionDescriptorType::CombinedImageSampler: descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
                    case ShaderReflectionDescriptorType::UniformBuffer:        descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
                    
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

    Vector<VkDescriptorSetLayout> descriptorSetLayoutHandles;
    descriptorSetLayoutHandles.SetCountFromTemplate(maxSetIndex + 1, VK_NULL_HANDLE);

    Vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    for (int32 setIndex = 0; setIndex <= maxSetIndex; ++setIndex)
    {
        if (bindingsPerSet[setIndex].IsEmpty())
        {
            // The indices corresponding to each descriptor set layout are implicitly determined by the order in which elements
            // appear in the 'VkPipelineLayoutCreateInfo::pSetLayouts' array. If a descriptor set index is not used by the shader,
            // it is acceptable for the corresponding set layout to be VK_NULL_HANDLE.
            // 
            // For example, if a shader uses descriptor sets 0, 1 and 3, in order to correctly implement this "gap" feature we must
            // set the descriptor set layout corresponding to index 2 to VK_NULL_HANDLE. */
            descriptorSetLayoutHandles[setIndex] = VK_NULL_HANDLE;
            continue;
        }

        VulkanDescriptorSetLayout& descriptorSetLayout = m_DescriptorSetLayouts[setIndex];

        descriptorSetLayoutBindings.Clear();
        descriptorSetLayoutBindings.EnsureCapacity(bindingsPerSet[setIndex].Count());
        for (const auto& [bindingIndex, binding] : bindingsPerSet[setIndex])
        {
            SE_ASSERT(!descriptorSetLayout.BindingDescriptorTypes.Contains(bindingIndex));
            descriptorSetLayout.BindingDescriptorTypes.Add(bindingIndex, binding.descriptorType);
            descriptorSetLayoutBindings.Add(binding);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = (uint32)descriptorSetLayoutBindings.Count();
        descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.Elements();

        SE_VULKAN_CHECK(vkCreateDescriptorSetLayout(g_VulkanDriver->GetDevice(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout.Handle));
        descriptorSetLayoutHandles[setIndex] = descriptorSetLayout.Handle;
    }

    // Create the pipeline layout.
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = (uint32)descriptorSetLayoutHandles.Count();
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayoutHandles.Elements();

    SE_VULKAN_CHECK(vkCreatePipelineLayout(g_VulkanDriver->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));

    // Create the descriptor set manager.
    m_DescriptorSetManager = CreateOwn<VulkanDescriptorSetManager>(AdoptWeakRef(this), m_DescriptorSetLayouts);
}

VulkanShader::~VulkanShader()
{
    // Dispatch the pre-destroy callbacks.
    DispatchCallbacksOfType(RHIObjectCallbackType::PreDestroy);

    // Destroy the descriptor set manager.
    m_DescriptorSetManager = nullptr;

    // Destroy the descriptor set layouts.
    for (const auto& [setIndex, setLayout] : m_DescriptorSetLayouts)
        vkDestroyDescriptorSetLayout(g_VulkanDriver->GetDevice(), setLayout.Handle, nullptr);
    m_DescriptorSetLayouts.ClearAndShrink();

    // Destroy the pipeline layout.
    vkDestroyPipelineLayout(g_VulkanDriver->GetDevice(), m_PipelineLayout, nullptr);
    m_PipelineLayout = VK_NULL_HANDLE;

    // Destroy the shader modules.
    for (Module& module : m_Modules)
        vkDestroyShaderModule(g_VulkanDriver->GetDevice(), module.Handle, nullptr);
    m_Modules.ClearAndShrink();
}

}
