// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanCommandPool.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

VulkanCommandPool::VulkanCommandPool(const VulkanCommandPoolInfo& info)
    : m_Handle(VK_NULL_HANDLE)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = info.QueueFamilyIndex;

    SE_VULKAN_CHECK(vkCreateCommandPool(g_VulkanDriver->GetDevice(), &commandPoolCreateInfo, nullptr, &m_Handle));

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = m_Handle;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = info.CommandBufferCount;

    m_UnusedCommandBuffers.SetCountDefaulted(info.CommandBufferCount);
    SE_VULKAN_CHECK(vkAllocateCommandBuffers(g_VulkanDriver->GetDevice(), &allocateInfo, m_UnusedCommandBuffers.Elements()));

    /* Avoid possible memory allocations during 'AcquireCommandBuffer' and 'RetireCommandBuffer'. */
    m_UnusedCommandBuffers.EnsureCapacity(info.CommandBufferCount);
}

VulkanCommandPool::~VulkanCommandPool()
{
    if (m_InUseCommandBuffers.HasElements())
    {
        SE_LOG_ERROR("[Vulkan] command pool is destroyed (unreferenced) but still has in-use command buffers!");
        SE_ASSERT_NOT_REACHED;
    }

    /* Free command buffers allocted from this pool. */
    vkFreeCommandBuffers(g_VulkanDriver->GetDevice(), m_Handle, (uint32)m_UnusedCommandBuffers.Count(), m_UnusedCommandBuffers.Elements());
    
    /* Destroy the command pool. */
    vkDestroyCommandPool(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;
}

VkCommandBuffer VulkanCommandPool::AcquireCommandBuffer()
{
    SE_ENSURE(m_UnusedCommandBuffers.HasElements());
    const VkCommandBuffer commandBufferHandle = m_UnusedCommandBuffers.Last();
    m_UnusedCommandBuffers.PopBack();
    m_InUseCommandBuffers.Add(commandBufferHandle);
    return commandBufferHandle;
}

void VulkanCommandPool::RetireCommandBuffer(VkCommandBuffer commandBufferHandle)
{
    SE_ENSURE(m_InUseCommandBuffers.Contains(commandBufferHandle));
    m_InUseCommandBuffers.RemoveUnchecked(commandBufferHandle);
    m_UnusedCommandBuffers.Add(commandBufferHandle);
}

}
