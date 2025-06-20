// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Core/Platform/PlatformCoreInclude.h>

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
