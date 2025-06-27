// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanShader.h>

namespace SE
{

VulkanShader::VulkanShader(const ShaderInfo& info)
{
    for (const ShaderStageInfo& shaderStage : info.Stages)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = shaderStage.Bytecode.size();
        shaderModuleCreateInfo.pCode = (const uint32*)shaderStage.Bytecode.data();

        Module module = {};
        if (VkResult result = vkCreateShaderModule(g_VulkanDriver->GetDevice(), &shaderModuleCreateInfo, nullptr, &module.Handle); result != VK_SUCCESS)
        {
            SE_LOG_ERROR("Failed to create the [Vulkan] shader module! (Result: %d)", result);
            continue;
        }

        switch (shaderStage.Stage)
        {
            case ShaderStage::Vertex:   module.Stage = VK_SHADER_STAGE_VERTEX_BIT;   break;
            case ShaderStage::Fragment: module.Stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
            default:                    SE_ASSERT_NOT_REACHED;                       break;
        }

        m_Modules.push_back(module);
    }
}

VulkanShader::~VulkanShader()
{
    for (Module& module : m_Modules)
    {
        vkDestroyShaderModule(g_VulkanDriver->GetDevice(), module.Handle, nullptr);
        module = {};
    }

    m_Modules.clear();
}

}
