/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Math/IntVector.h>
#include <Core/Math/Vector.h>
#include <Engine/Application/Events/Event.h>
#include <Engine/Application/KeyCode.h>

namespace SE
{

class Input
{
public:
    SHOOTER_API static bool initialize();
    SHOOTER_API static void shutdown();

    SHOOTER_API static void on_update();
    SHOOTER_API static void on_event(const Event& in_event);

public:
    NODISCARD SHOOTER_API static bool is_key_down(KeyCode key_code);
    NODISCARD SHOOTER_API static bool was_key_pressed_this_frame(KeyCode key_code);

    NODISCARD SHOOTER_API static bool is_key_up(KeyCode key_code);
    NODISCARD SHOOTER_API static bool was_key_released_this_frame(KeyCode key_code);

    NODISCARD SHOOTER_API static bool is_mouse_button_down(MouseButton mouse_button);
    NODISCARD SHOOTER_API static bool was_mouse_button_pressed_this_frame(MouseButton mouse_button);

    NODISCARD SHOOTER_API static bool is_mouse_button_up(MouseButton mouse_button);
    NODISCARD SHOOTER_API static bool was_mouse_button_released_this_frame(MouseButton mouse_button);

    NODISCARD SHOOTER_API static float get_mouse_offset_x();
    NODISCARD SHOOTER_API static float get_mouse_offset_y();

    NODISCARD ALWAYS_INLINE static Vector2 get_mouse_offset()
    {
        Vector2 mouse_offset = Vector2(get_mouse_offset_x(), get_mouse_offset_y());
        return mouse_offset;
    }

    NODISCARD SHOOTER_API static float get_mouse_wheel_scroll_offset();

public:
    SHOOTER_API static void clear_all_key_states();
    SHOOTER_API static void clear_all_mouse_button_states();
    SHOOTER_API static void clear_all_states();

private:
    static void platform_initialize();
    static void platform_shutdown();

    static IntVector2 platform_get_mouse_position();
};

} // namespace SE
