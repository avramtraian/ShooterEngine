// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/CommandList.h>
#include <Runtime/Renderer/RHI/ShaderResource.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

#include <unordered_map>
#include <vector>

namespace SE
{

struct VulkanDescriptorSetLayout
{
    VkDescriptorSetLayout Handle;
    std::unordered_map<uint32, VkDescriptorType> BindingDescriptorTypes;
};

enum class DescriptorSetCompatibility : uint8
{
    Incompatible,
    Compatible,
    // WorthUpdating,
};

class VulkanDescriptorSet
{
public:
    VulkanDescriptorSet(VkDescriptorPool descriptorPool, uint32 setIndex, const VulkanDescriptorSetLayout& setLayout);
    ~VulkanDescriptorSet();

public:
    NODISCARD FORCEINLINE VkDescriptorSet GetHandle() const { return m_DescriptorSet; }
    NODISCARD FORCEINLINE VkDescriptorSetLayout GetLayout() const { return m_DescriptorSetLayout.Handle; }
    NODISCARD FORCEINLINE uint32 GetSetIndex() const { return m_SetIndex; }
    NODISCARD FORCEINLINE bool IsLocked() const { return (m_LockCount > 0); }

    NODISCARD DescriptorSetCompatibility IsCompatibleWithBindings(const std::unordered_map<uint32, RefPtr<ShaderResource>>& bindings) const;
    void UpdateBindings(const std::unordered_map<uint32, RefPtr<ShaderResource>>& bindings);
    
    NODISCARD std::vector<uint32> GetMissingBindingIndices() const;
    NODISCARD bool IsComplete() const;

    void IncrementLockCount();
    void DecrementLockCount();

private:
    VkDescriptorSet m_DescriptorSet;
    uint32 m_SetIndex;
    VkDescriptorPool m_DescriptorPool;
    uint32 m_LockCount;

    // NOTE(Traian): The descriptor set layout is provided by the shader when creating the descriptor set. As the shader is the
    // one that manages all descriptor sets (and caches them accordingly) this handle will never be destroyed as long as a descriptor
    // set still uses it (as their lifetime is smaller than their parent shaders).
    const VulkanDescriptorSetLayout& m_DescriptorSetLayout;

    struct BindingResource
    {
        WeakRefPtr<ShaderResource> Resource;
        ShaderResourcePreDestroyCallback PreDestroyCallback;
    };

    // NOTE(Traian): The following container maps binding indices to the resoures that are actually bound at that location.
    // We explicitly don't hold references to those resources because these // descriptor sets are usually very aggressively cached,
    // which can cause unneccessary reference holding. Shader resources have a callback mechanism that is triggered before the
    // resource is invalidated/destroyed. We listen to these callbacks and invalidate/update the descriptor set accordingly.
    std::unordered_map<uint32, BindingResource> m_BindingResources;

    // NOTE(Traian): Is used the ensure that the resources are not destroyed (as these ref pointers maintain references) and
    // only has elements stored in it when the descriptor set is bounded.
    std::vector<StrongRefPtr<ShaderResource>> m_LockedResources;
};

}
