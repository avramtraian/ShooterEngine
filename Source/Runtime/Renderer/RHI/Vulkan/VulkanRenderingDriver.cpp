// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Log.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCommandList.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanShader.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace SE
{

SHOOTER_API VulkanRenderingDriver* g_VulkanDriver;

static VkBool32 VulkanDebugMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void*)
{
    switch (messageSeverity)
    {
        // case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        // {
        //     SE_LOG_ERROR("VULKAN WARNING: %s", callbackData->pMessage);
        //     break;
        // }
        // case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        // {
        //     SE_LOG_ERROR("VULKAN WARNING: %s", callbackData->pMessage);
        //     break;
        // }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        {
            SE_LOG_WARN("VULKAN WARNING: %s", callbackData->pMessage);
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        {
            SE_LOG_ERROR("VULKAN ERROR: %s", callbackData->pMessage);
            break;
        }
    }

    return VK_FALSE;
}

enum class InstanceComponentType
{
    ExtensionRequired,
    ExtensionOptional,
    LayerRequired,
    LayerOptional,
};

struct InstanceComponent
{
    InstanceComponentType Type;
    std::string Name;
};

// Returns whether or not all required extensions and layers are available.
// The out parameters contain all available extensions/layers that were requested - if the return value is
// not false, these vectors are guaranteed to contain the extensions/layers in the required lists.
static bool CheckInstanceComponents(const std::vector<InstanceComponent>& components, std::vector<const char*>& outExtensions, std::vector<const char*>& outLayers)
{
    /* Query available instance extensions. */
    uint32 availableExtensionCount = 0;
    std::vector<VkExtensionProperties> availableExtensions;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr) == VK_SUCCESS)
    {
        availableExtensions.resize(availableExtensionCount);
        SE_VULKAN_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data()));
        SE_ENSURE(availableExtensions.size() == availableExtensionCount);
    }

    /* Query available instance layers. */
    uint32 availableLayerCount = 0;
    std::vector<VkLayerProperties> availableLayers;
    if (vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr) == VK_SUCCESS)
    {
        availableLayers.resize(availableLayerCount);
        SE_VULKAN_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));
        SE_ENSURE(availableLayers.size() == availableLayerCount);
    }

    outExtensions.clear();
    outLayers.clear();

    std::vector<std::string> missingExtensionsRequired;
    std::vector<std::string> missingExtensionsOptional;
    std::vector<std::string> missingLayersRequired;
    std::vector<std::string> missingLayersOptional;

    for (const InstanceComponent& component : components)
    {
        if (component.Type == InstanceComponentType::ExtensionRequired || component.Type == InstanceComponentType::ExtensionOptional)
        {
            bool extensionIsAvailable = false;
            for (const VkExtensionProperties& availableExtension : availableExtensions)
            {
                if (strcmp(availableExtension.extensionName, component.Name.c_str()) == 0)
                {
                    extensionIsAvailable = true;
                    break;
                }
            }

            if (extensionIsAvailable)
            {
                outExtensions.push_back(component.Name.c_str());
            }
            else
            {
                if (component.Type == InstanceComponentType::ExtensionRequired)
                    missingExtensionsRequired.push_back(component.Name);
                if (component.Type == InstanceComponentType::ExtensionOptional)
                    missingExtensionsOptional.push_back(component.Name);
            }
        }
        if (component.Type == InstanceComponentType::LayerRequired || component.Type == InstanceComponentType::LayerOptional)
        {
            bool layerIsAvailable = false;
            for (const VkLayerProperties& availableLayer : availableLayers)
            {
                if (strcmp(availableLayer.layerName, component.Name.c_str()) == 0)
                {
                    layerIsAvailable = true;
                    break;
                }
            }

            if (layerIsAvailable)
            {
                outLayers.push_back(component.Name.c_str());
            }
            else
            {
                if (component.Type == InstanceComponentType::LayerRequired)
                    missingLayersRequired.push_back(component.Name);
                if (component.Type == InstanceComponentType::LayerOptional)
                    missingLayersOptional.push_back(component.Name);
            }
        }
    }

    if (!missingExtensionsRequired.empty())
    {
        SE_LOG_ERROR("The following required instance extensions are missing:");
        for (const std::string& extensionName : missingExtensionsRequired)
        {
            SE_LOG_ERROR("  %s", extensionName.c_str());
        }
    }
    if (!missingExtensionsOptional.empty())
    {
        SE_LOG_WARN("The following optional instance extensions are missing:");
        for (const std::string& extensionName : missingExtensionsOptional)
        {
            SE_LOG_WARN("  %s", extensionName.c_str());
        }
    }
    if (!missingLayersRequired.empty())
    {
        SE_LOG_ERROR("The following required instance layers are missing:");
        for (const std::string& layerName : missingLayersRequired)
        {
            SE_LOG_ERROR("  %s", layerName.c_str());
        }
    }
    if (!missingLayersOptional.empty())
    {
        SE_LOG_WARN("The following optional instance layers are missing:");
        for (const std::string& layerName : missingLayersOptional)
        {
            SE_LOG_WARN("  %s", layerName.c_str());
        }
    }

    /* Check if there are no required extensions or layers missing. */
    const bool allRequiredComponentsAreAvailable = missingExtensionsRequired.empty() && missingLayersRequired.empty();
    return allRequiredComponentsAreAvailable;
}

bool VulkanRenderingDriver::CreateInstance()
{
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
    debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerCreateInfo.pfnUserCallback = VulkanDebugMessengerCallback;

    std::vector<InstanceComponent> instanceComponents;

    instanceComponents.push_back({ InstanceComponentType::ExtensionRequired, VK_KHR_SURFACE_EXTENSION_NAME });
    instanceComponents.push_back({ InstanceComponentType::ExtensionRequired, "VK_KHR_win32_surface" });

#define SE_ENABLE_VULKAN_VALIDATION 1
#if SE_ENABLE_VULKAN_VALIDATION
    instanceComponents.push_back({ InstanceComponentType::ExtensionOptional, "VK_EXT_debug_utils" });
    instanceComponents.push_back({ InstanceComponentType::LayerOptional,     "VK_LAYER_KHRONOS_validation" });
#endif // FG_ENABLE_VULKAN_VALIDATION

    std::vector<const char*> enabledExtensions;
    std::vector<const char*> enabledLayers;
    if (!CheckInstanceComponents(instanceComponents, enabledExtensions, enabledLayers))
    {
        /* NOTE(Traian): Not all required extensions/layers were available. */
        return false;
    }

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = &debugMessengerCreateInfo;
    instanceCreateInfo.enabledExtensionCount = (uint32)enabledExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
    instanceCreateInfo.enabledLayerCount = (uint32)enabledLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = enabledLayers.data();
    
    if (VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance); result != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the Vulkan instance! (Result: %d)", result);
        return false;
    }

    if (!enabledExtensions.empty() || !enabledLayers.empty())
    {
        SE_LOG_INFO("The [Vulkan] instance was created with the following extensions and layers:");
        for (const char* extensionName : enabledExtensions)
        {
            SE_LOG_INFO("  (Extension) %s", extensionName);
        }
        for (const char* layerName : enabledLayers)
        {
            SE_LOG_INFO("  (Layer) %s", layerName);
        }
    }

    using PFN_vkCreateDebugUtilsMessengerEXT = VkResult(*)(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
    PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
    if (pfnVkCreateDebugUtilsMessengerEXT)
    {
        /* NOTE(Traian): As the debug messenger is not a critical component of the engine we don't stop the initialization
         * of the rendering driver backend just because its creation has failed. */
        pfnVkCreateDebugUtilsMessengerEXT(m_Instance, &debugMessengerCreateInfo, nullptr, &m_DebugMessenger);
    }

    return true;
}

bool VulkanRenderingDriver::PickPhysicalDevice()
{
    uint32 physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDeviceHandles;
    if (vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr) == VK_SUCCESS)
    {
        physicalDeviceHandles.resize(physicalDeviceCount);
        SE_VULKAN_CHECK(vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDeviceHandles.data()));
        SE_ENSURE(physicalDeviceCount == physicalDeviceHandles.size());
    }

    std::vector<PhysicalDevice> physicalDevices;
    physicalDevices.reserve(physicalDeviceHandles.size());

    for (VkPhysicalDevice physicalDeviceHandle : physicalDeviceHandles)
    {
        PhysicalDevice physicalDevice = {};
        physicalDevice.Handle = physicalDeviceHandle;

        /* Get physical device properties. */
        vkGetPhysicalDeviceProperties(physicalDeviceHandle, &physicalDevice.Properties);
        
        /* Get physical device features. */
        vkGetPhysicalDeviceFeatures(physicalDeviceHandle, &physicalDevice.Features);

        /* Get physical device queue family properties. */
        uint32 queueFamilyPropertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle, &queueFamilyPropertyCount, nullptr);
        physicalDevice.QueueFamilyProperties.resize(queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle, &queueFamilyPropertyCount, physicalDevice.QueueFamilyProperties.data());
        SE_ENSURE(queueFamilyPropertyCount == physicalDevice.QueueFamilyProperties.size());

        /* Get physical device available extensions. */
        uint32 extensionCount = 0;
        if (vkEnumerateDeviceExtensionProperties(physicalDevice.Handle, nullptr, &extensionCount, nullptr) == VK_SUCCESS)
        {
            physicalDevice.AvailableExtensions.resize(extensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice.Handle, nullptr, &extensionCount, physicalDevice.AvailableExtensions.data());
            SE_ENSURE(physicalDevice.AvailableExtensions.size() == extensionCount);
        }

        /* Get memory properties. */
        vkGetPhysicalDeviceMemoryProperties(physicalDevice.Handle, &physicalDevice.MemoryProperties);

        physicalDevices.push_back(physicalDevice);
    }

    if (physicalDevices.empty())
    {
        SE_LOG_ERROR("There are no [Vulkan] physical devices available!");
        return false;
    }

    /* NOTE(Traian): Use the "best performing and very complex algorithm" of picking the first 
     * physical device returned by the 'vkEnumeratePhysicalDevices' function. */
    m_PhysicalDevice = physicalDevices[0];

    SE_LOG_INFO("Picked '%s' as the [Vulkan] physical device.", m_PhysicalDevice.Properties.deviceName);
    return true;
}

bool VulkanRenderingDriver::FindQueueFamilyIndices()
{
    /* NOTE(Traian): Because device creation happens before creating the main window and 
     * other windows can be created dynamically at runtime, there is no other way to test if
     * a queue family supports presentation functionality other than to create a dummy window,
     * a dummy surface and to test against them. */
    VkSurfaceKHR dummySurface = VK_NULL_HANDLE;

#if SE_PLATFORM_WIN64
    WNDCLASSA windowClass = {};
    windowClass.hInstance = GetModuleHandle(nullptr);
    windowClass.lpszClassName = "ShooterVulkanDummyWindowClass";
    windowClass.lpfnWndProc = DefWindowProcA;
    RegisterClassA(&windowClass);
    
    HWND dummyWindow = CreateWindowA(
        "ShooterVulkanDummyWindowClass", "", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    surfaceCreateInfo.hwnd = dummyWindow;
    SE_VULKAN_CHECK(vkCreateWin32SurfaceKHR(m_Instance, &surfaceCreateInfo, nullptr, &dummySurface));
#endif // SE_PLATFORM_WIN64

    enum class QueueFamilyType : uint8_t
    {
        Graphics,
        Compute,
        Transfer,
        Present,
    };

    struct QueueFamily
    {
        uint32 Index;
        uint32 SupportedTypeCount;
    };

    std::unordered_map<QueueFamilyType, std::vector<QueueFamily>> queueFamilyMap;
    for (uint32 familyIndex = 0; familyIndex < m_PhysicalDevice.QueueFamilyProperties.size(); ++familyIndex)
    {
        const VkQueueFamilyProperties& properties = m_PhysicalDevice.QueueFamilyProperties[familyIndex];
        QueueFamily queueFamily = {};
        queueFamily.Index = familyIndex;
        queueFamily.SupportedTypeCount = 0;

        VkBool32 presentSupport = false;
        SE_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice.Handle, familyIndex, dummySurface, &presentSupport));

        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) { queueFamily.SupportedTypeCount++; }
        if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT) { queueFamily.SupportedTypeCount++; }
        if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT)  { queueFamily.SupportedTypeCount++; }
        if (presentSupport)                                { queueFamily.SupportedTypeCount++; }

        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) { queueFamilyMap[QueueFamilyType::Graphics].push_back(queueFamily); }
        if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT) { queueFamilyMap[QueueFamilyType::Transfer].push_back(queueFamily); }
        if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT)  { queueFamilyMap[QueueFamilyType::Compute].push_back(queueFamily); }
        if (presentSupport)                                { queueFamilyMap[QueueFamilyType::Present].push_back(queueFamily); }
    }

    /* Destroy the dummy surface. */
    vkDestroySurfaceKHR(m_Instance, dummySurface, nullptr);
    dummySurface = VK_NULL_HANDLE;

    auto getLowestSupportedTypeCountFamilyIndex = [&](QueueFamilyType type) -> uint32
        {
            uint32 lowestCount = UINT32_MAX;
            uint32 lowestCountEntryIndex = 0;

            const std::vector<QueueFamily>& queueFamilies = queueFamilyMap[type];
            for (uint32 index = 0; index < queueFamilies.size(); ++index)
            {
                if (queueFamilies[index].SupportedTypeCount < lowestCount)
                {
                    lowestCount = queueFamilies[index].SupportedTypeCount;
                    lowestCountEntryIndex = index;
                }
            }

            return queueFamilies[lowestCountEntryIndex].Index;
        };

    if (queueFamilyMap[QueueFamilyType::Graphics].empty())
        return false;
    m_QueueFamilyIndices.Graphics = getLowestSupportedTypeCountFamilyIndex(QueueFamilyType::Graphics);

    if (queueFamilyMap[QueueFamilyType::Transfer].empty())
        return false;
    m_QueueFamilyIndices.Transfer = getLowestSupportedTypeCountFamilyIndex(QueueFamilyType::Transfer);

    if (queueFamilyMap[QueueFamilyType::Compute].empty())
        return false;
    m_QueueFamilyIndices.Compute = getLowestSupportedTypeCountFamilyIndex(QueueFamilyType::Compute);

    if (queueFamilyMap[QueueFamilyType::Present].empty())
        return false;
    m_QueueFamilyIndices.Present = getLowestSupportedTypeCountFamilyIndex(QueueFamilyType::Present);

    return true;
}

bool VulkanRenderingDriver::CreateLogicalDevice()
{
    std::vector<const char*> enabledExtensions;
    enabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    std::vector<const char*> missingExtensions;
    for (const char* extension : enabledExtensions)
    {
        bool found = false;
        for (const VkExtensionProperties& availableExtension : m_PhysicalDevice.AvailableExtensions)
        {
            if (strcmp(availableExtension.extensionName, extension) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
            missingExtensions.push_back(extension);
    }

    if (!missingExtensions.empty())
    {
        SE_LOG_ERROR("The following [Vulkan] logical device extensions are missing:");
        for (const char* missingExtension : missingExtensions)
        {
            SE_LOG_ERROR("  %s", missingExtension);
        }

        /* Not all device extensions are available! Aborting. */
        return false;
    }

    std::unordered_set<uint32> uniqueQueueFamilyIndices;
    uniqueQueueFamilyIndices.insert({
        (uint32)m_QueueFamilyIndices.Graphics,
        (uint32)m_QueueFamilyIndices.Transfer,
        (uint32)m_QueueFamilyIndices.Compute,
        (uint32)m_QueueFamilyIndices.Present,
    });

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());

    const float queuePriorities[] = { 1.0F };
    for (uint32 queueFamilyIndex : uniqueQueueFamilyIndices)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.queueCount = SE_ARRAY_COUNT(queuePriorities);
        queueCreateInfo.pQueuePriorities = queuePriorities;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.enabledExtensionCount = (uint32)enabledExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
    deviceCreateInfo.queueCreateInfoCount = (uint32)queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = nullptr; /* NOTE(Traian): Don't enable any features that aren't absolutely mandatory/necessary. */

    if (VkResult result = vkCreateDevice(m_PhysicalDevice.Handle, &deviceCreateInfo, nullptr, &m_LogicalDevice); result != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the [Vulkan] logical device! (Result: %d)", result);
        return false;
    }

    if (!enabledExtensions.empty())
    {
        SE_LOG_INFO("The [Vulkan] logical device was created with the following extensions:");
        for (const char* extension : enabledExtensions)
        {
            SE_LOG_INFO("  %s", extension);
        }
    }

    return true;
}

bool VulkanRenderingDriver::CreateQueues()
{
    vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.Graphics, 0, &m_QueueGraphics);
    vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.Transfer, 0, &m_QueueTransfer);
    vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.Compute, 0, &m_QueueCompute);
    vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.Present, 0, &m_QueuePresent);

    return true;
}

bool VulkanRenderingDriver::CreateDescriptorPool()
{
    const uint32 descriptorPoolMaxSets = 1024;
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;

    VkDescriptorPoolSize& uniformBufferPool = descriptorPoolSizes.emplace_back();
    uniformBufferPool.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferPool.descriptorCount = 512;

    VkDescriptorPoolSize& combinedImageSamplerPool = descriptorPoolSizes.emplace_back();
    combinedImageSamplerPool.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    combinedImageSamplerPool.descriptorCount = 512;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.poolSizeCount = (uint32)descriptorPoolSizes.size();
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    descriptorPoolCreateInfo.maxSets = descriptorPoolMaxSets;

    if (vkCreateDescriptorPool(m_LogicalDevice, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
    {
        SE_LOG_ERROR("Failed to create the [Vulkan] descriptor pool!");
        return false;
    }

    return true;
}

bool VulkanRenderingDriver::CreateCommandPools(const RenderingDriverInfo& info)
{
    m_CommandPoolForQueueFamilyIndex[m_QueueFamilyIndices.Graphics] = CreateRef<VulkanCommandPool>(VulkanCommandPoolInfo()
        .SetCommandBufferCount(16)
        .SetQueueFamilyIndex(m_QueueFamilyIndices.Graphics)
    );
    m_CommandPoolForQueueFamilyIndex[m_QueueFamilyIndices.Transfer] = CreateRef<VulkanCommandPool>(VulkanCommandPoolInfo()
        .SetCommandBufferCount(8)
        .SetQueueFamilyIndex(m_QueueFamilyIndices.Transfer)
    );
    m_CommandPoolForQueueFamilyIndex[m_QueueFamilyIndices.Compute] = CreateRef<VulkanCommandPool>(VulkanCommandPoolInfo()
        .SetCommandBufferCount(4)
        .SetQueueFamilyIndex(m_QueueFamilyIndices.Compute)
    );

    return true;
}

bool VulkanRenderingDriver::InitializeBackend(const RenderingDriverInfo& info)
{
    SE_LOG_INFO("Initializing the [Vulkan] rendering driver backend...");
    if (g_VulkanDriver)
    {
        SE_LOG_ERROR("The [Vulkan] rendering driver backend was already initialized!");
        return false;
    }

    /* Initialize the driver. */
    if (!CreateInstance())         { return false; }
    if (!PickPhysicalDevice())     { return false; }
    if (!FindQueueFamilyIndices()) { return false; }
    if (!CreateLogicalDevice())    { return false; }
    if (!CreateQueues())           { return false; }
    if (!CreateDescriptorPool())   { return false; }

    g_VulkanDriver = this;

    /* Initialize rendering subsystems. */
    if (!CreateCommandPools(info)) { return false; }
    
    return true;
}

void VulkanRenderingDriver::ShutdownBackend()
{
    if (g_VulkanDriver != this)
    {
        SE_LOG_WARN("Trying to shutdown the [Vulkan] rendering driver backend but it has already been shutted down!");
        return;
    }

    /* Ensure all GPU operations have finished and all resources can safely be destroyed. */
    WaitForDeviceIdle();

    /* Destroy synchronization objects. */
    {
        SE_ASSERT(m_FencePool.InUse.empty());
        for (VkFence fence : m_FencePool.Unused)
            vkDestroyFence(m_LogicalDevice, fence, nullptr);

        SE_ASSERT(m_SemaphorePool.InUse.empty());
        for (VkSemaphore semaphore : m_SemaphorePool.Unused)
            vkDestroySemaphore(m_LogicalDevice, semaphore, nullptr);
    }

    // Destroy the descriptor pool.
    vkDestroyDescriptorPool(m_LogicalDevice, m_DescriptorPool, nullptr);
    m_DescriptorPool = VK_NULL_HANDLE;

    /* Destroy command pools. */
    m_CommandPoolForQueueFamilyIndex.clear();

    /* Destroy logical device. */
    vkDestroyDevice(m_LogicalDevice, nullptr);
    m_LogicalDevice = VK_NULL_HANDLE;

    /* Destroy the debug messenger. */
    if (m_DebugMessenger != VK_NULL_HANDLE)
    {
        using PFN_vkDestroyDebugUtilsMessengerEXT = void(*)(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);
        PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
        if (pfnVkDestroyDebugUtilsMessengerEXT)
        {
            pfnVkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
            m_DebugMessenger = VK_NULL_HANDLE;
        }
    }

    /* Destroy the instance/ */
    vkDestroyInstance(m_Instance, nullptr);
    m_Instance = VK_NULL_HANDLE;

    g_VulkanDriver = nullptr;
}

RefPtr<RenderingSurface> VulkanRenderingDriver::CreateSurface(const RenderingSurfaceInfo& info)
{
    return CreateRef<VulkanRenderingSurface>(info);
}

RefPtr<CommandList> VulkanRenderingDriver::CreateCommandList(const CommandListInfo& info)
{
    return CreateRef<VulkanCommandList>(info);
}

RefPtr<IndexBuffer> VulkanRenderingDriver::CreateIndexBuffer(const IndexBufferInfo& info)
{
    return CreateRef<VulkanIndexBuffer>(info);
}

RefPtr<RenderPass> VulkanRenderingDriver::CreateRenderPass(const RenderPassInfo& info)
{
    return CreateRef<VulkanRenderPass>(info);
}

RefPtr<Texture2D> VulkanRenderingDriver::CreateTexture2D(const Texture2DInfo& info)
{
    return CreateRef<VulkanStorageTexture2D>(info);
}

RefPtr<Shader> VulkanRenderingDriver::CreateShader(const ShaderInfo& info)
{
    return CreateRef<VulkanShader>(info);
}

RefPtr<VertexBuffer> VulkanRenderingDriver::CreateVertexBuffer(const VertexBufferInfo& info)
{
    return CreateRef<VulkanVertexBuffer>(info);
}


RefPtr<UniformBuffer> VulkanRenderingDriver::CreateUniformBuffer(const UniformBufferInfo& info)
{
    return CreateRef<VulkanUniformBuffer>(info);
}

FenceHandle VulkanRenderingDriver::AcquireFence()
{
    if (!m_FencePool.HasUnusedObjects())
    {
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkFence fence = VK_NULL_HANDLE;
        SE_VULKAN_CHECK(vkCreateFence(m_LogicalDevice, &fenceCreateInfo, nullptr, &fence));
        m_FencePool.AddUnusedObject(fence);
    }

    VkFence fence = m_FencePool.Acquire();
    SE_ASSERT(IsFenceSignaled((FenceHandle)fence));
    return (FenceHandle)fence;
}

void VulkanRenderingDriver::RetireFence(FenceHandle fenceHandle)
{
    if (!IsFenceSignaled((FenceHandle)fenceHandle))
    {
        SE_LOG_ERROR("Trying to retire a fence that isn't signaled!");
        SE_ASSERT_NOT_REACHED;
        return;
    }

    VkFence fence = (VkFence)fenceHandle;
    m_FencePool.Retire(fence);
}

SemaphoreHandle VulkanRenderingDriver::AcquireSemaphore()
{
    if (!m_SemaphorePool.HasUnusedObjects())
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkSemaphore semaphore = VK_NULL_HANDLE;
        SE_VULKAN_CHECK(vkCreateSemaphore(m_LogicalDevice, &semaphoreCreateInfo, nullptr, &semaphore));
        m_SemaphorePool.AddUnusedObject(semaphore);
    }

    VkSemaphore semaphore = m_SemaphorePool.Acquire();
    return (SemaphoreHandle)semaphore;
}

void VulkanRenderingDriver::RetireSemaphore(SemaphoreHandle semaphoreHandle)
{
    VkSemaphore semaphore = (VkSemaphore)semaphoreHandle;
    m_SemaphorePool.Retire(semaphore);
}

void VulkanRenderingDriver::ExecuteCommandList(const RefPtr<CommandList>& commandList, const CommandListExecuteInfo& executeInfo)
{
    RefPtr<VulkanCommandList> vulkanCommandList = commandList.As<VulkanCommandList>();
    VkCommandBuffer commandBufferHandle = vulkanCommandList->GetHandle();

    std::vector<VkSemaphore> waitSemaphores;
    waitSemaphores.reserve(executeInfo.WaitSemaphores.size());
    std::vector<VkPipelineStageFlags> waitStageMasks;
    waitStageMasks.reserve(executeInfo.WaitSemaphores.size());

    for (uint32 semaphoreIndex = 0; semaphoreIndex < executeInfo.WaitSemaphores.size(); ++semaphoreIndex)
    {
        VkPipelineStageFlags waitStageMask = 0;
        if (executeInfo.WaitStageBits[semaphoreIndex] & PIPELINE_STAGE_TOP_OF_PIPE_BIT)             { waitStageMask |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; }
        if (executeInfo.WaitStageBits[semaphoreIndex] & PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT) { waitStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; }

        waitSemaphores.push_back((VkSemaphore)executeInfo.WaitSemaphores[semaphoreIndex]);
        waitStageMasks.push_back(waitStageMask);
    }

    std::vector<VkSemaphore> signalSemaphores;
    signalSemaphores.reserve(executeInfo.SignalSemaphores.size());
    for (SemaphoreHandle semaphore : executeInfo.SignalSemaphores)
        signalSemaphores.push_back((VkSemaphore)semaphore);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = (uint32)waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStageMasks.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBufferHandle;
    submitInfo.signalSemaphoreCount = (uint32)signalSemaphores.size();
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    VkQueue submisionQueue = VK_NULL_HANDLE;
    switch (commandList->GetFamily())
    {
        case CommandListFamily::Graphics: submisionQueue = m_QueueGraphics; break;
        case CommandListFamily::Transfer: submisionQueue = m_QueueTransfer; break;
        case CommandListFamily::Compute:  submisionQueue = m_QueueCompute; break;
        default: SE_ASSERT_NOT_REACHED;
    }

    SE_ASSERT(submisionQueue != VK_NULL_HANDLE);
    SE_VULKAN_CHECK(vkQueueSubmit(submisionQueue, 1, &submitInfo, (VkFence)executeInfo.SignalFence));
}


void VulkanRenderingDriver::ExecuteCommandListAndWait(const RefPtr<CommandList>& commandList, const CommandListExecuteInfo& executeInfo)
{
    if (executeInfo.SignalFence)
    {
        SE_LOG_WARN("Executing a command list with automatic waiting doesn't accept a fence to signal!");
    }

    CommandListExecuteInfo newExecuteInfo = executeInfo;
    newExecuteInfo.SignalFence = AcquireFence();
    ResetFence(newExecuteInfo.SignalFence);

    ExecuteCommandList(commandList, newExecuteInfo);
    WaitForFence(newExecuteInfo.SignalFence, UINT64_MAX);
    RetireFence(newExecuteInfo.SignalFence);
}

void VulkanRenderingDriver::WaitForFence(FenceHandle fence, uint64 timeout)
{
    VkFence fenceHandle = (VkFence)fence;
    SE_VULKAN_CHECK(vkWaitForFences(m_LogicalDevice, 1, &fenceHandle, VK_TRUE, timeout));
}

bool VulkanRenderingDriver::IsFenceSignaled(FenceHandle fence)
{
    const VkResult fenceStatus = vkGetFenceStatus(m_LogicalDevice, (VkFence)fence);
    return (fenceStatus == VK_SUCCESS);
}

void VulkanRenderingDriver::ResetFence(FenceHandle fence)
{
    VkFence fenceHandle = (VkFence)fence;
    SE_VULKAN_CHECK(vkResetFences(m_LogicalDevice, 1, &fenceHandle));
}


void VulkanRenderingDriver::WaitForDeviceIdle()
{
    SE_VULKAN_CHECK(vkDeviceWaitIdle(m_LogicalDevice));
}

}
