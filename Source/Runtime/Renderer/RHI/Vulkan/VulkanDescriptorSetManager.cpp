// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSetManager.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

VulkanDescriptorSetManager::VulkanDescriptorSetManager(const WeakRefPtr<VulkanShader>& parentShader, const HashMap<uint32, VulkanDescriptorSetLayout>& setLayouts)
    : m_ParentShader(parentShader)
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

    m_SetCaches.ClearAndShrink();
}

Vector<VulkanDescriptorSet*> VulkanDescriptorSetManager::AcquireDescriptorSets(const ShaderResourcesBindPack& bindPack)
{
    HashMap<uint32, HashMap<uint32, RefPtr<ShaderResource>>> resourceSets;
    for (const auto& texture : bindPack.Textures)
        resourceSets[texture.SetIndex][texture.BindingIndex] = texture.Texture;
    for (const auto& uniformBuffer : bindPack.UniformBuffers)
        resourceSets[uniformBuffer.SetIndex][uniformBuffer.BindingIndex] = uniformBuffer.Buffer;

    // List of descriptor sets that are required to be bound to the pipeline in order to provide access to all
    // resources specified by the given bind info structure.
    Vector<VulkanDescriptorSet*> descriptorSets;

    for (const auto& [setIndex, setBindings] : resourceSets)
    {
        SE_ASSERT(m_SetCaches.Contains(setIndex));
        DescriptorSetCache& setCache = m_SetCaches.At(setIndex);
        bool wasCachedSetFound = false;
    
        // TODO(Traian): Use a "smarter" method to determine if a cached set is compatible with the current bind pack. Checking each
        // cached descriptor set every time 'AcquireDescriptorSets' is invoked can have serious performance costs when a lot of shader
        // resources are available. Use some sort of hashing instead.
        for (OwnPtr<VulkanDescriptorSet>& cachedSet : setCache.Sets)
        {
            if (cachedSet->IsCompatibleWithBindings(setBindings) == DescriptorSetCompatibility::Compatible)
            {
                descriptorSets.Add(cachedSet.Get());
                wasCachedSetFound = true;
                break;
            }
        }

        if (!wasCachedSetFound)
        {
            // TODO(Traian): Depending on the number of cached descriptor sets and usage patterns, try update or invalidate
            // an existing descriptor set instead of creating a new one.
            SE_ASSERT(setCache.Sets.Count() < 1024);

            VkDescriptorPool descriptorPool = g_VulkanDriver->GetDescriptorPool();
            auto set = CreateOwn<VulkanDescriptorSet>(descriptorPool, setIndex, setCache.Layout, m_ParentShader);
            set->UpdateBindings(setBindings);
            descriptorSets.Add(set.Get());
            setCache.Sets.Add(std::move(set));
        }
    }

    for (VulkanDescriptorSet* descriptorSet : descriptorSets)
    {
        // NOTE(Traian): We expect the bind pack to have no missing binding indices.
        SE_ASSERT(descriptorSet->IsComplete());
    }

    return descriptorSets;
}

}
