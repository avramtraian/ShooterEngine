// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/CommandList.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSet.h>

namespace SE
{

class VulkanDescriptorSetManager
{
public:
    struct DescriptorSetCache
    {
        VulkanDescriptorSetLayout Layout;
        std::vector<std::unique_ptr<VulkanDescriptorSet>> Sets;
    };

public:
    VulkanDescriptorSetManager(const std::unordered_map<uint32, VulkanDescriptorSetLayout>& setLayouts);
    ~VulkanDescriptorSetManager();

    NODISCARD std::vector<VulkanDescriptorSet*> AcquireDescriptorSets(const ShaderResourcesBindPack& bindPack);

private:
    std::unordered_map<uint32, DescriptorSetCache> m_SetCaches;
};

}
