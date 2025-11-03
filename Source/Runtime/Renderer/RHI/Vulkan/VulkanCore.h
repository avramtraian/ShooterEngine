// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashSet.h>
#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Core/Platform/PlatformCoreInclude.h>

#include <vulkan/vulkan.h>
#if SE_PLATFORM_WINDOWS
    #include <vulkan/vulkan_win32.h>
#endif // SE_PLATFORM_WINDOWS

#define SE_VULKAN_CHECK(...)                                         \
    if ((__VA_ARGS__) != VK_SUCCESS)                                 \
    {                                                                \
        SE_LOG_ERROR("Vulkan expression failed!\n%s", #__VA_ARGS__); \
        SE_ASSERT_NOT_REACHED;                                       \
    }

namespace SE
{

NODISCARD ALWAYS_INLINE static String VulkanFormatToString(VkFormat format)
{
    switch (format)
    {
#define _SE_CASE(formatName) \
    case formatName: return VIEW(#formatName);
        _SE_CASE(VK_FORMAT_B8G8R8A8_UNORM);
        _SE_CASE(VK_FORMAT_B8G8R8A8_SNORM);
        _SE_CASE(VK_FORMAT_B8G8R8A8_UINT);
        _SE_CASE(VK_FORMAT_B8G8R8A8_SINT);
        _SE_CASE(VK_FORMAT_B8G8R8A8_SRGB);
#undef _SE_CASE
    }

    /* TODO(Traian): Extend the switch statement above to include more image
     * formats such this return case is never reached! */
    return VIEW("<unstringifyable>");
}

/* Forward declarations of RHI interfaces. */
class VulkanCommandList;
class VulkanIndexBuffer;
class VulkanRenderPass;
class VulkanRenderingDriver;
class VulkanRenderingSurface;
class VulkanShader;
class VulkanTexture2D;
class VulkanVertexBuffer;

/* Forward declarations of Vulkan-specific classes. */
class VulkanCommandPool;
class VulkanFramebuffer;
class VulkanPipeline;

/* Forward declarations of information structures. */
struct VulkanCommandPoolInfo;

template<typename VulkanHandleType>
struct VulkanObjectPool
{
public:
    NODISCARD ALWAYS_INLINE uint32 GetNumberOfUnusedObjects() const { return (uint32)Unused.Count(); }
    NODISCARD ALWAYS_INLINE uint32 GetNumberOfInUseObjects() const { return (uint32)InUse.Count(); }
    NODISCARD ALWAYS_INLINE bool HasUnusedObjects() const { return (GetNumberOfUnusedObjects() > 0); }

public:
    NODISCARD ALWAYS_INLINE VulkanHandleType Acquire()
    {
        if (Unused.IsEmpty())
            return VK_NULL_HANDLE;

        VulkanHandleType handle = Unused.Last();
        Unused.PopBack();
        InUse.Add(handle);

        return handle;
    }

    ALWAYS_INLINE void Retire(VulkanHandleType handle)
    {
        SE_ENSURE(InUse.Contains(handle));
        InUse.RemoveUnchecked(handle);
        Unused.Add(handle);
    }

public:
    void AddUnusedObject(VulkanHandleType handle) { Unused.Add(handle); }

    void AddInUseObject(VulkanHandleType handle) { InUse.Add(handle); }

public:
    Vector<VulkanHandleType>  Unused;
    HashSet<VulkanHandleType> InUse;
};

} // namespace SE
