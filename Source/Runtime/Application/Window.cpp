// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Window.h>

#if SE_PLATFORM_WINDOWS
    #include <Runtime/Application/Platform/Windows/WindowsWindow.h>
    namespace SE { using NativeWindow = WindowsWindow; }
#endif // SE_PLATFORM_WINDOWS

namespace SE
{

RefPtr<Window> Window::Create(const WindowInfo& info)
{
    return CreateRef<NativeWindow>(info);
}

}
