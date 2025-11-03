// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Containers/LockPtr.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Renderer/RHI/CommandList.h>
#include <Runtime/Renderer/RHI/ShaderResource.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

struct VulkanDescriptorSetLayout
{
    VkDescriptorSetLayout Handle;
    HashMap<uint32, VkDescriptorType> BindingDescriptorTypes;
};

enum class DescriptorSetCompatibility : uint8
{
    Incompatible,
    Compatible,
    // WorthUpdating,
};

class VulkanDescriptorSet final : public LockCounted
{
public:
    VulkanDescriptorSet(VkDescriptorPool descriptorPool, uint32 setIndex, const VulkanDescriptorSetLayout& setLayout, const WeakRefPtr<VulkanShader>& parentShader);
    ~VulkanDescriptorSet();

public:
    NODISCARD FORCEINLINE VkDescriptorSet GetHandle() const { return m_DescriptorSet; }
    NODISCARD FORCEINLINE VkDescriptorSetLayout GetLayout() const { return m_DescriptorSetLayout.Handle; }
    NODISCARD FORCEINLINE uint32 GetSetIndex() const { return m_SetIndex; }

    NODISCARD DescriptorSetCompatibility IsCompatibleWithBindings(const HashMap<uint32, RefPtr<ShaderResource>>& bindings) const;
    void UpdateBindings(const HashMap<uint32, RefPtr<ShaderResource>>& bindings);
    
    NODISCARD Vector<uint32> GetMissingBindingIndices() const;
    NODISCARD bool IsComplete() const;

protected:
    virtual void OnLock() override;
    virtual void OnUnlock() override;

private:
    VkDescriptorSet m_DescriptorSet;
    uint32 m_SetIndex;
    VkDescriptorPool m_DescriptorPool;

    // NOTE(Traian): Since descriptor sets are owned by a descriptor set manager which in turn is owned by the parent shader, we can
    // be certain that as long as this descriptor set is alive (the instance hasn't been deleted) the shader is also alive. We use
    // this weak-strong reference holding mechanism to ensure that as long as this descriptor set is locked, the shader will not
    // be destroyed.
    WeakRefPtr<VulkanShader> m_ParentShader;
    StrongRefPtr<VulkanShader> m_LockedParentShader;

    // NOTE(Traian): The descriptor set layout is provided by the shader when creating the descriptor set. As the shader is the
    // one that manages all descriptor sets (and caches them accordingly) this handle will never be destroyed as long as a descriptor
    // set still uses it (as their lifetime is smaller than their parent shaders).
    const VulkanDescriptorSetLayout& m_DescriptorSetLayout;

    struct BindingResource
    {
        WeakRefPtr<ShaderResource> Resource;
        RHIObjectCallback PreDestroyCallback;
    };

    // NOTE(Traian): The following container maps binding indices to the resoures that are actually bound At that location.
    // We explicitly don't hold references to those resources because these // descriptor sets are usually very aggressively cached,
    // which can cause unneccessary reference holding. Shader resources have a callback mechanism that is triggered before the
    // resource is invalidated/destroyed. We listen to these callbacks and invalidate/update the descriptor set accordingly.
    HashMap<uint32, BindingResource> m_BindingResources;

    // NOTE(Traian): Is used the ensure that the resources are not destroyed (as these ref pointers maintain references) and
    // only has elements stored in it when the descriptor set is bounded.
    Vector<StrongRefPtr<ShaderResource>> m_LockedResources;
};

}
