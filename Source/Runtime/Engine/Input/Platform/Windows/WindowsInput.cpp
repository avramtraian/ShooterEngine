/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Platform/Windows/WindowsHeadersConditional.h>
#include <Engine/Input/Input.h>

namespace SE
{

struct WindowsInputData
{
    u32 primary_monitor_height { 0 };
};

static WindowsInputData* s_windows_input;

void Input::platform_initialize()
{
    s_windows_input = new WindowsInputData();

    HMONITOR primary_monitor = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    
    MONITORINFO monitor_info = {};
    monitor_info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(primary_monitor, &monitor_info);
    s_windows_input->primary_monitor_height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
}

void Input::platform_shutdown()
{
    delete s_windows_input;
    s_windows_input = nullptr;
}

IntVector2 Input::platform_get_mouse_position()
{
    POINT mouse_position = {};
    GetCursorPos(&mouse_position);
    // NOTE: The coordinates provieded by the Win32 API have the origin in the top-left corner of the
    // screen area. However, the engine requires the coordinate system origin to be in the bottom-left corner.
    // The magic off-by-one constant is wierd.
    return IntVector2(mouse_position.x, s_windows_input->primary_monitor_height - mouse_position.y - 1);
}

} // namespace SE
