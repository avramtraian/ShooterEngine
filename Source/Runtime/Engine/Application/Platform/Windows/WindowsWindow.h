/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Platform/Windows/WindowsHeaders.h"
#include "Engine/Application/Window.h"

namespace SE {

class WindowsWindow final : public Window {
public:
    static LRESULT window_procedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

public:
    WindowsWindow() = default;

    virtual bool initialize(const WindowInfo& window_info) override;
    virtual ~WindowsWindow();

    virtual void pump_messages() override;

    virtual void* get_native_handle() const { return m_native_handle; }
    virtual bool should_close() const { return m_should_close; }

private:
    HWND m_native_handle = nullptr;
    bool m_should_close = false;

    u32 m_client_area_width = 0;
    u32 m_client_area_height = 0;
    i32 m_client_area_position_x = 0;
    i32 m_client_area_position_y = 0;
};

} // namespace SE