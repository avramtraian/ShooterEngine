// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreDefines.h>

#if SE_PLATFORM_WIN64
    #include <Runtime/Application/Platform/Windows/WindowsWindow.h>
    
namespace SE
{
    using WindowMode = WindowsWindowMode;
    using WindowInfo = WindowsWindowInfo;
    using Window     = WindowsWindow;
}
#endif // SE_PLATFORM_WIN64
