/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/OwnPtr.h"
#include "Core/Containers/String.h"

namespace SE {

struct WindowInfo {
    u32 width = 800;
    u32 height = 500;
    i32 position_x = 600;
    i32 position_y = 600;
    String title = "Unnamed Window"sv;
    bool start_maximized = false;
    bool start_minimized = false;
    bool start_fullscreen = false;
};

class Window {
public:
    SHOOTER_API static OwnPtr<Window> instantiate();

public:
    virtual bool initialize(const WindowInfo& window_info) = 0;
    virtual ~Window() = default;

    virtual void pump_messages() = 0;

    virtual void* get_native_handle() const = 0;
    virtual bool should_close() const = 0;

    virtual u32 get_width() const = 0;
    virtual u32 get_height() const = 0;
    virtual i32 get_position_x() const = 0;
    virtual i32 get_position_y() const = 0;

protected:
    Window() = default;
};

} // namespace SE