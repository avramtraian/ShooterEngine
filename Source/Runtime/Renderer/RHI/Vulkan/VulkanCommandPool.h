// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashSet.h>
#include <Runtime/Core/Containers/RefPtr.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

struct VulkanCommandPoolInfo
{
public:
    uint32 CommandBufferCount { 0 };
    int32 QueueFamilyIndex { -1 };

public:
    inline VulkanCommandPoolInfo& SetCommandBufferCount (uint32 count)           { CommandBufferCount = count;          return *this; }
    inline VulkanCommandPoolInfo& SetQueueFamilyIndex   (int32 queueFamilyIndex) { QueueFamilyIndex = queueFamilyIndex; return *this; }
};

class VulkanCommandPool : public RefCounted
{
public:
    VulkanCommandPool(const VulkanCommandPoolInfo& info);
    virtual ~VulkanCommandPool() override;

    NODISCARD ALWAYS_INLINE uint32 GetInUseCommandBufferCount() const { return (uint32)m_InUseCommandBuffers.Count(); }
    NODISCARD ALWAYS_INLINE uint32 GetUnusedCommandBufferCount() const { return (uint32)m_UnusedCommandBuffers.Count(); }
    NODISCARD ALWAYS_INLINE uint32 GetTotalCommandBufferCount() const { return GetInUseCommandBufferCount() + GetUnusedCommandBufferCount(); }

    NODISCARD VkCommandBuffer AcquireCommandBuffer();
    void RetireCommandBuffer(VkCommandBuffer commandBufferHandle);

private:
    VkCommandPool m_Handle;
    HashSet<VkCommandBuffer> m_InUseCommandBuffers;
    Vector<VkCommandBuffer> m_UnusedCommandBuffers;
};

}
