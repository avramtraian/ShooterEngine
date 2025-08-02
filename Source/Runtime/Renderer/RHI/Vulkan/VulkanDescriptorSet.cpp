// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSet.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

VulkanDescriptorSet::VulkanDescriptorSet(VkDescriptorPool descriptorPool, uint32 setIndex, const VulkanDescriptorSetLayout& setLayout)
    : m_DescriptorSet(VK_NULL_HANDLE)
    , m_SetIndex(setIndex)
    , m_DescriptorPool(descriptorPool)
    , m_LockCount(0)
    , m_DescriptorSetLayout(setLayout)
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &m_DescriptorSetLayout.Handle;

    // Allocate the descriptor set from the provided pool.
    SE_VULKAN_CHECK(vkAllocateDescriptorSets(g_VulkanDriver->GetDevice(), &descriptorSetAllocateInfo, &m_DescriptorSet));
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    // Free the descriptor set from the provided pool.
    vkFreeDescriptorSets(g_VulkanDriver->GetDevice(), m_DescriptorPool, 1, &m_DescriptorSet);
    m_DescriptorSet = VK_NULL_HANDLE;
    m_DescriptorPool = VK_NULL_HANDLE;
}

DescriptorSetCompatibility VulkanDescriptorSet::IsCompatibleWithBindings(const std::unordered_map<uint32, ShaderResource*>& bindings) const
{
    for (const auto& [bindingIndex, resource] : bindings)
    {
        // Check if the binding exists.
        auto currentBindingIt = m_BindingResources.find(bindingIndex);
        if (currentBindingIt == m_BindingResources.end())
            return DescriptorSetCompatibility::Incompatible;

        // Check if the resources bound at the given binding are the same.
        ShaderResource* currentResource = (*currentBindingIt).second;
        if (resource != currentResource)
            return DescriptorSetCompatibility::Incompatible;
    }

    return DescriptorSetCompatibility::Compatible;
}

void VulkanDescriptorSet::Invalidate(const std::unordered_map<uint32, ShaderResource*>& bindings)
{
    if (IsLocked())
    {
        SE_LOG_ERROR("Trying to invalidate a descriptor set while it is locked!");
        SE_ASSERT_NOT_REACHED;
    }

    std::vector<VkWriteDescriptorSet> descriptorWrites;
    for (auto [bindingIndex, resource] : bindings)
    {
        if (m_BindingResources.contains(bindingIndex) && m_BindingResources.at(bindingIndex) == resource)
            continue;
        m_BindingResources[bindingIndex] = resource;

        SE_ASSERT(m_DescriptorSetLayout.BindingDescriptorTypes.contains(bindingIndex));
        const VkDescriptorType descriptorType = m_DescriptorSetLayout.BindingDescriptorTypes.at(bindingIndex);

        VkWriteDescriptorSet& descriptorWrite = descriptorWrites.emplace_back();
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorSet;
        descriptorWrite.dstBinding = bindingIndex;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = descriptorType;

        if (descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
        {
            VulkanTexture2D* textureResource = (VulkanTexture2D*)resource;
            VkDescriptorImageInfo descriptorImageInfo = {};
            descriptorImageInfo.sampler = textureResource->GetSampler().Handle;
            descriptorImageInfo.imageView = textureResource->GetHandle().View;
            descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptorWrite.pImageInfo = &descriptorImageInfo;
        }
    }

    vkUpdateDescriptorSets(g_VulkanDriver->GetDevice(), (uint32)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void VulkanDescriptorSet::IncrementLockCount()
{
    if (!IsLocked())
    {
        SE_ASSERT(m_LockedResources.empty());
        m_LockedResources.reserve(m_BindingResources.size());

        for (auto [bindingIndex, resource] : m_BindingResources)
        {
            RefPtr<ShaderResource> trackedResource = AdoptRef<ShaderResource>(resource);
            m_LockedResources.push_back(trackedResource);
        }
    }

    ++m_LockCount;
}

void VulkanDescriptorSet::DecrementLockCount()
{
    SE_ASSERT(IsLocked());
    --m_LockCount;

    if (m_LockCount == 0)
        m_LockedResources.clear();
}

}
