// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Renderer/RHI/RenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCommandPool.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanPipeline.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.h>

namespace SE
{

class VulkanRenderingDriver : public RenderingDriver
{
public:
    struct PhysicalDevice
    {
        VkPhysicalDevice Handle { VK_NULL_HANDLE };
        VkPhysicalDeviceProperties Properties;
        VkPhysicalDeviceFeatures Features;
        Vector<VkQueueFamilyProperties> QueueFamilyProperties;
        Vector<VkExtensionProperties> AvailableExtensions;
        VkPhysicalDeviceMemoryProperties MemoryProperties;
    };

    struct QueueFamilyIndices
    {
        int32 Graphics { -1 };
        int32 Transfer { -1 };
        int32 Compute  { -1 };
        int32 Present  { -1 };
    };

public:
    VulkanRenderingDriver() = default;
    virtual ~VulkanRenderingDriver() override = default;

    NODISCARD ALWAYS_INLINE VkInstance GetInstance() const { return m_Instance; }
    NODISCARD ALWAYS_INLINE VkDevice GetDevice() const { return m_LogicalDevice; }
    NODISCARD ALWAYS_INLINE const PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
    NODISCARD ALWAYS_INLINE const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

    NODISCARD ALWAYS_INLINE VkQueue GetGraphicsQueue() const { return m_QueueGraphics; }
    NODISCARD ALWAYS_INLINE VkQueue GetTransferQueue() const { return m_QueueTransfer; }
    NODISCARD ALWAYS_INLINE VkQueue GetComputeQueue() const { return m_QueueCompute; }
    NODISCARD ALWAYS_INLINE VkQueue GetPresentQueue() const { return m_QueuePresent; }

    NODISCARD ALWAYS_INLINE RefPtr<VulkanCommandPool> GetCommandPool(uint32 queueFamilyIndex) const
    {
        SE_ENSURE(m_CommandPoolForQueueFamilyIndex.Contains(queueFamilyIndex));
        return m_CommandPoolForQueueFamilyIndex.At(queueFamilyIndex);
    }

    NODISCARD ALWAYS_INLINE VkDescriptorPool GetDescriptorPool() const { return m_DescriptorPool; }

public:
    virtual RefPtr<RenderingSurface> CreateSurface(const RenderingSurfaceInfo& info) override;
    
    virtual RefPtr<CommandList> CreateCommandList(const CommandListInfo& info) override;
    virtual RefPtr<IndexBuffer> CreateIndexBuffer(const IndexBufferInfo& info) override;
    virtual RefPtr<RenderPass> CreateRenderPass(const RenderPassInfo& info) override;
    virtual RefPtr<Shader> CreateShader(const ShaderInfo& info) override;
    virtual RefPtr<VertexBuffer> CreateVertexBuffer(const VertexBufferInfo& info) override;

    virtual RefPtr<Texture2D> CreateTexture2D(const Texture2DInfo& info) override;
    virtual RefPtr<UniformBuffer> CreateUniformBuffer(const UniformBufferInfo& info) override;

    virtual FenceHandle AcquireFence() override;
    virtual void RetireFence(FenceHandle fenceHandle) override;
    virtual SemaphoreHandle AcquireSemaphore() override;
    virtual void RetireSemaphore(SemaphoreHandle semaphoreHandle) override;

public:
    virtual void ExecuteCommandList(const RefPtr<CommandList>& commandList, const CommandListExecuteInfo& executeInfo) override;
    virtual void ExecuteCommandListAndWait(const RefPtr<CommandList>& commandList, const CommandListExecuteInfo& executeInfo) override;

    virtual void WaitForFence(FenceHandle fence, uint64 timeout) override;
    virtual bool IsFenceSignaled(FenceHandle fence) override;
    virtual void ResetFence(FenceHandle fence) override;
    virtual void WaitForDeviceIdle() override;

private:
    virtual bool InitializeBackend(const RenderingDriverInfo& info) override;
    virtual void ShutdownBackend() override;

    bool CreateInstance();
    bool PickPhysicalDevice();
    bool FindQueueFamilyIndices();
    bool CreateLogicalDevice();
    bool CreateQueues();
    bool CreateDescriptorPool();

    bool CreateCommandPools(const RenderingDriverInfo& info);

private:
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    PhysicalDevice m_PhysicalDevice;
    QueueFamilyIndices m_QueueFamilyIndices;
    VkDevice m_LogicalDevice;

    VkQueue m_QueueGraphics;
    VkQueue m_QueueTransfer;
    VkQueue m_QueueCompute;
    VkQueue m_QueuePresent;

    /* NOTE(Traian): The driver currently only creates one commnd pool per queue family index.
     * Once we will start extending the renderer to be multi-threaded, this architecture must
     * be expanded to allow for multiple command pools per queue family index (one for each
     * active thread for example). */
    HashMap<uint32, RefPtr<VulkanCommandPool>> m_CommandPoolForQueueFamilyIndex;

    VulkanObjectPool<VkFence> m_FencePool;
    VulkanObjectPool<VkSemaphore> m_SemaphorePool;

    VkDescriptorPool m_DescriptorPool;
};

RUNTIME_API extern VulkanRenderingDriver* g_VulkanDriver;

}
