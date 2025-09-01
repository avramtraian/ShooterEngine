// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/OwnPtr.h>
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
        Vector<OwnPtr<VulkanDescriptorSet>> Sets;
    };

public:
    VulkanDescriptorSetManager(const WeakRefPtr<VulkanShader>& parentShader, const HashMap<uint32, VulkanDescriptorSetLayout>& setLayouts);
    ~VulkanDescriptorSetManager();

    NODISCARD Vector<VulkanDescriptorSet*> AcquireDescriptorSets(const ShaderResourcesBindPack& bindPack);

private:
    WeakRefPtr<VulkanShader> m_ParentShader;
    HashMap<uint32, DescriptorSetCache> m_SetCaches;
};

}
