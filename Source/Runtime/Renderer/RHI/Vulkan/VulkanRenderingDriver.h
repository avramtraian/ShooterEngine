// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

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

public:
    virtual std::unique_ptr<RenderingSurface> CreateSurface(const RenderingSurfaceInfo& info) override;

private:
    virtual bool InitializeBackend(const RenderingDriverInfo& info) override;
    virtual void ShutdownBackend() override;

    bool CreateInstance();
    bool PickPhysicalDevice();
    bool FindQueueFamilyIndices();
    bool CreateLogicalDevice();

private:
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    PhysicalDevice m_PhysicalDevice;
    QueueFamilyIndices m_QueueFamilyIndices;
    VkDevice m_LogicalDevice;
};

SHOOTER_API extern VulkanRenderingDriver* g_VulkanDriver;

}
