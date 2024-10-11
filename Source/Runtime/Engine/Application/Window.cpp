/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Application/Window.h>

#if SE_PLATFORM_WINDOWS
    #include <Engine/Application/Platform/Windows/WindowsWindow.h>
#endif // Platform switch.

namespace SE
{

OwnPtr<Window> Window::instantiate()
{
#if SE_PLATFORM_WINDOWS
    Window* window = new WindowsWindow();
    return adopt_own(window);
#endif // Platform switch.
}

} // namespace SE
