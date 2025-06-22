// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Core/Platform/PlatformCoreInclude.h>

#include <string>

#include <vulkan/vulkan.h>

#if SE_PLATFORM_WIN64
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h>
    #include <vulkan/vulkan_win32.h>
#endif // SE_PLATFORM_WIN64

#define SE_VULKAN_CHECK(...)                                          \
    if ((__VA_ARGS__) != VK_SUCCESS) {                                \
        SE_LOG_ERROR("Vulkan expression failed!\n%s", #__VA_ARGS__);  \
        SE_ASSERT_NOT_REACHED;                                        \
    }

NODISCARD FORCEINLINE static std::string VulkanFormatToString(VkFormat format)
{

    switch (format)
    {
#define _SE_CASE(formatName) case formatName: return #formatName;
        _SE_CASE(VK_FORMAT_B8G8R8A8_UNORM);
        _SE_CASE(VK_FORMAT_B8G8R8A8_SNORM);
        _SE_CASE(VK_FORMAT_B8G8R8A8_UINT);
        _SE_CASE(VK_FORMAT_B8G8R8A8_SINT);
        _SE_CASE(VK_FORMAT_B8G8R8A8_SRGB);
#undef _SE_CASE
    }

    /* TODO(Traian): Extend the switch statement above to include more image
     * formats such this return case is never reached! */
    return "<unstringifyable>";
}
