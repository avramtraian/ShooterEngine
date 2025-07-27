// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCommandPool.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanPipeline.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.h>

#include <unordered_map>
#include <vector>

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
        std::vector<VkQueueFamilyProperties> QueueFamilyProperties;
        std::vector<VkExtensionProperties> AvailableExtensions;
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

    NODISCARD FORCEINLINE VkInstance GetInstance() const { return m_Instance; }
    NODISCARD FORCEINLINE VkDevice GetDevice() const { return m_LogicalDevice; }
    NODISCARD FORCEINLINE const PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }

    NODISCARD FORCEINLINE std::shared_ptr<VulkanCommandPool> GetCommandPool(uint32 queueFamilyIndex) const
    {
        SE_ENSURE(m_CommandPoolForQueueFamilyIndex.contains(queueFamilyIndex));
        return m_CommandPoolForQueueFamilyIndex.at(queueFamilyIndex);
    }

public:
    virtual std::unique_ptr<RenderingSurface> CreateSurface(const RenderingSurfaceInfo& info) override;
    virtual std::shared_ptr<CommandList> CreateCommandList(const CommandListInfo& info) override;
    virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer(const IndexBufferInfo& info) override;
    virtual std::shared_ptr<Shader> CreateShader(const ShaderInfo& info) override;
    virtual std::shared_ptr<Texture2D> CreateTexture2D(const Texture2DInfo& info) override;
    virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer(const VertexBufferInfo& info) override;

    virtual FenceHandle AcquireFence() override;
    virtual void RetireFence(FenceHandle fenceHandle) override;
    virtual SemaphoreHandle AcquireSemaphore() override;
    virtual void RetireSemaphore(SemaphoreHandle semaphoreHandle) override;
public:
    /* NOTE(Traian): Vulkan render pass objects are created on demand. There is no RHI-abstract class
     * that a user can create that represents a render pass. Instead, a big pool of already created
     * render passes is managed by the driver. When the user begins a render pass, the 'RenderPassInfo'
     * structure is "hashed" and if a compatible render pass already exists that's great. Otheriwse,
     * the driver creates a new render pass using 'vkCreateRenderPass'. */
    VkRenderPass AcquireRenderPass(const RenderPassInfo& renderPassInfo);
    void RetireRenderPass(VkRenderPass renderPassHandle);

    /* NOTE(Traian): Similar behaviour to how render passes are managed. Read the above documentation/comments. */
    VkFramebuffer AcquireFramebuffer(VkRenderPass renderPassHandle, const RenderPassInfo& renderPassInfo);
    void RetireFramebuffer(VkFramebuffer framebufferHandle);
    virtual void WaitForFence(FenceHandle fence, uint64 timeout) override;
    virtual bool IsFenceSignaled(FenceHandle fence) override;
    virtual void ResetFence(FenceHandle fence) override;

private:
    virtual bool InitializeBackend(const RenderingDriverInfo& info) override;
    virtual void ShutdownBackend() override;

    bool CreateInstance();
    bool PickPhysicalDevice();
    bool FindQueueFamilyIndices();
    bool CreateLogicalDevice();
    bool CreateCommandPools(const RenderingDriverInfo& info);

private:
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    PhysicalDevice m_PhysicalDevice;
    QueueFamilyIndices m_QueueFamilyIndices;
    VkDevice m_LogicalDevice;

    /* NOTE(Traian): The driver currently only creates one commnd pool per queue family index.
     * Once we will start extending the renderer to be multi-threaded, this architecture must
     * be expanded to allow for multiple command pools per queue family index (one for each
     * active thread for example). */
    std::unordered_map<uint32, std::shared_ptr<VulkanCommandPool>> m_CommandPoolForQueueFamilyIndex;

    struct RenderPassCache
    {
        std::vector<std::unique_ptr<VulkanRenderPass>> CreatedObjects;
        std::unordered_set<uint32> InUseIndices;
        std::vector<uint32> UnusedIndices;
    };
    RenderPassCache m_RenderPassCache;

    struct FramebufferCache
    {
        std::vector<std::unique_ptr<VulkanFramebuffer>> CreatedObjects;
        std::unordered_set<uint32> InUseIndices;
        std::vector<uint32> UnusedIndices;
    };
    FramebufferCache m_FramebufferCache;
    VulkanObjectPool<VkFence> m_FencePool;
    VulkanObjectPool<VkSemaphore> m_SemaphorePool;
};

SHOOTER_API extern VulkanRenderingDriver* g_VulkanDriver;

}
