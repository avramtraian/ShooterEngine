// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreDefines.h>

#if SE_PLATFORM_WIN64
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h>
#else
    #error Trying to include the Windows headers but they are not available!
#endif // SE_PLATFORM_WIN64
