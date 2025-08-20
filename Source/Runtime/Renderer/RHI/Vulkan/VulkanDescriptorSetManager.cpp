// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSetManager.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

VulkanDescriptorSetManager::VulkanDescriptorSetManager(const std::unordered_map<uint32, VulkanDescriptorSetLayout>& setLayouts)
{
    for (const auto& [setIndex, setLayout] : setLayouts)
        m_SetCaches[setIndex].Layout = setLayout;
}

VulkanDescriptorSetManager::~VulkanDescriptorSetManager()
{
    for (const auto& [setIndex, setCache] : m_SetCaches)
    {
        for (const auto& descriptorSet : setCache.Sets)
        {
            if (descriptorSet->IsLocked())
            {
                SE_LOG_ERROR("Trying to destroy a descriptor set manager that caches descriptor sets that are locked!");
                SE_ASSERT_NOT_REACHED;
            }
        }
    }

    m_SetCaches.clear();
}

std::vector<VulkanDescriptorSet*> VulkanDescriptorSetManager::AcquireDescriptorSets(const ShaderResourcesBindPack& bindPack)
{
    std::unordered_map<uint32, std::unordered_map<uint32, RefPtr<ShaderResource>>> resourceSets;
    for (const auto& texture : bindPack.Textures)
        resourceSets[texture.SetIndex][texture.BindingIndex] = texture.Texture;
    for (const auto& uniformBuffer : bindPack.UniformBuffers)
        resourceSets[uniformBuffer.SetIndex][uniformBuffer.BindingIndex] = uniformBuffer.Buffer;

    // List of descriptor sets that are required to be bound to the pipeline in order to provide access to all
    // resources specified by the given bind info structure.
    std::vector<VulkanDescriptorSet*> descriptorSets;

    for (const auto& [setIndex, setBindings] : resourceSets)
    {
        SE_ASSERT(m_SetCaches.contains(setIndex));
        DescriptorSetCache& setCache = m_SetCaches.at(setIndex);
        bool wasCachedSetFound = false;
    
        // TODO(Traian): Use a "smarter" method to determine if a cached set is compatible with the current bind pack. Checking each
        // cached descriptor set every time 'AcquireDescriptorSets' is invoked can have serious performance costs when a lot of shader
        // resources are available. Use some sort of hashing instead.
        for (std::unique_ptr<VulkanDescriptorSet>& cachedSet : setCache.Sets)
        {
            if (cachedSet->IsCompatibleWithBindings(setBindings) == DescriptorSetCompatibility::Compatible)
            {
                descriptorSets.push_back(cachedSet.get());
                wasCachedSetFound = true;
                break;
            }
        }

        if (!wasCachedSetFound)
        {
            // TODO(Traian): Depending on the number of cached descriptor sets and usage patterns, try update or invalidate
            // an existing descriptor set instead of creating a new one.
            SE_ASSERT(setCache.Sets.size() < 1024);

            VkDescriptorPool descriptorPool = g_VulkanDriver->GetDescriptorPool();
            auto set = std::make_unique<VulkanDescriptorSet>(descriptorPool, setIndex, setCache.Layout);
            set->Invalidate(setBindings);
            descriptorSets.push_back(set.get());
            setCache.Sets.push_back(std::move(set));
        }
    }

    for (VulkanDescriptorSet* descriptorSet : descriptorSets)
        descriptorSet->IncrementLockCount();


    return descriptorSets;
}

}
