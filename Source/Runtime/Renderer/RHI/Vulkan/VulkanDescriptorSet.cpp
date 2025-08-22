// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSet.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

VulkanDescriptorSet::VulkanDescriptorSet(VkDescriptorPool descriptorPool, uint32 setIndex, const VulkanDescriptorSetLayout& setLayout, const WeakRefPtr<VulkanShader>& parentShader)
    : m_DescriptorSet(VK_NULL_HANDLE)
    , m_SetIndex(setIndex)
    , m_DescriptorPool(descriptorPool)
    , m_ParentShader(parentShader)
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

DescriptorSetCompatibility VulkanDescriptorSet::IsCompatibleWithBindings(const std::unordered_map<uint32, RefPtr<ShaderResource>>& bindings) const
{
    for (const auto& [bindingIndex, resource] : bindings)
    {
        // Check if the binding exists.
        auto currentBindingIt = m_BindingResources.find(bindingIndex);
        if (currentBindingIt == m_BindingResources.end())
            return DescriptorSetCompatibility::Incompatible;

        // Check if the resources bound at the given binding are the same.
        WeakRefPtr<ShaderResource> currentResource = (*currentBindingIt).second.Resource;
        if (resource != currentResource)
            return DescriptorSetCompatibility::Incompatible;
    }

    return DescriptorSetCompatibility::Compatible;
}

void VulkanDescriptorSet::UpdateBindings(const std::unordered_map<uint32, RefPtr<ShaderResource>>& bindings)
{
    if (IsLocked())
    {
        SE_LOG_ERROR("Trying to invalidate a descriptor set while it is locked!");
        SE_ASSERT_NOT_REACHED;
    }

    std::vector<VkWriteDescriptorSet> descriptorWrites;
    for (const auto& [bindingIndex, resource] : bindings)
    {
        // Check if the binding requires updating.
        if (m_BindingResources.contains(bindingIndex) && m_BindingResources.at(bindingIndex).Resource == resource)
            continue;

        m_BindingResources[bindingIndex].Resource = resource;
        m_BindingResources[bindingIndex].PreDestroyCallback = m_BindingResources[bindingIndex].Resource->AddPreDestroyCallback(
            [this, bindingIndex](ShaderResource& resourceToBeDestroyed)
            {
                // NOTE(Traian): Since the descriptor set holds strong references of the bound resources while it is locked, this
                // assert should never fail. If however it does fail, it most likely means that the 'm_BindingResources' and
                // 'm_LockedResources' vectors are not in sync! (which would be an internal error)
                SE_ASSERT(!IsLocked());

                // NOTE(Traian): Vulkan itself doesn't care that the binding contains handles to deleted resources, as long as that descriptor
                // set is not used. It is up to the application to ensure that the binding is updated to reference a valid new resource before
                // is it used by a command buffer.
                SE_ASSERT(m_BindingResources.contains(bindingIndex));
                m_BindingResources.erase(bindingIndex);
            }
        );

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
            RefPtr<VulkanTexture2D> vulkanTexture = resource.As<VulkanTexture2D>();
            SE_ASSERT(vulkanTexture->GetType() == VulkanStorageTexture2D::GetStaticType());
            RefPtr<VulkanStorageTexture2D> textureResource = vulkanTexture.As<VulkanStorageTexture2D>();

            VkDescriptorImageInfo descriptorImageInfo = {};
            descriptorImageInfo.sampler = textureResource->GetSampler().Handle;
            descriptorImageInfo.imageView = textureResource->GetHandle().View;
            descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptorWrite.pImageInfo = &descriptorImageInfo;
        }
        if (descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        {
            RefPtr<VulkanUniformBuffer> uniformBufferResource = resource.As<VulkanUniformBuffer>();
            VkDescriptorBufferInfo descriptorBufferInfo = {};
            descriptorBufferInfo.buffer = uniformBufferResource->GetHandle();
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = uniformBufferResource->GetBufferSize();
            descriptorWrite.pBufferInfo = &descriptorBufferInfo;
        }
    }

    vkUpdateDescriptorSets(g_VulkanDriver->GetDevice(), (uint32)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

std::vector<uint32> VulkanDescriptorSet::GetMissingBindingIndices() const
{
    std::vector<uint32> missingBindingIndices;
    for (const auto& [bindingIndex, descriptorType] : m_DescriptorSetLayout.BindingDescriptorTypes)
    {
        if (!m_BindingResources.contains(bindingIndex))
            missingBindingIndices.push_back(bindingIndex);
    }

    return missingBindingIndices;
}

bool VulkanDescriptorSet::IsComplete() const
{
    for (const auto& [bindingIndex, descriptorType] : m_DescriptorSetLayout.BindingDescriptorTypes)
    {
        if (!m_BindingResources.contains(bindingIndex))
            return false;
    }

    return true;
}

void VulkanDescriptorSet::OnLock()
{
    // Acquire strong reference for the parent shader (that owns the parent descriptor set managed and, in turn, this descriptor set).
    SE_ASSERT(!m_LockedParentShader.IsValid());
    SE_ASSERT(m_ParentShader.IsValid());
    m_LockedParentShader = m_ParentShader;

    // Acquire strong references for the resources used by the descriptor set.
    SE_ASSERT(m_LockedResources.empty());
    m_LockedResources.reserve(m_BindingResources.size());

    for (auto& [bindingIndex, resource] : m_BindingResources)
    {
        RefPtr<ShaderResource> trackedResource = RefPtr<ShaderResource>(resource.Resource);
        m_LockedResources.push_back(std::move(trackedResource));
    }
}

void VulkanDescriptorSet::OnUnlock()
{
    // Release the strong references for the resources.
    m_LockedResources.clear();

    // Release the strong reference for the parent shader.
    m_LockedParentShader.Release();
}

}
