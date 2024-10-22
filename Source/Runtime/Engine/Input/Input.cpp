/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/HashMap.h>
#include <Core/Containers/OwnPtr.h>
#include <Core/Containers/Vector.h>
#include <Core/Log.h>
#include <Engine/Application/Events/KeyEvents.h>
#include <Engine/Application/Events/MouseEvents.h>
#include <Engine/Input/Input.h>

namespace SE
{

struct KeyState
{
    bool is_down { false };
    bool was_pressed_this_frame { false };
    bool was_released_this_frame { false };
};

struct MouseButtonState
{
    bool is_down { false };
    bool was_pressed_this_frame { false };
    bool was_released_this_frame { false };
};

struct InputData
{
    Vector<OwnPtr<Event>> events_to_process;

    HashMap<KeyCode, KeyState> key_code_states;
    HashMap<MouseButton, MouseButtonState> mouse_button_states;

    Optional<IntVector2> last_frame_mouse_position;
    IntVector2 mouse_position_offset;
    float mouse_wheel_scroll_offset;
};

static InputData* s_input;

bool Input::initialize()
{
    if (s_input)
    {
        SE_LOG_ERROR("The input system has already been initialized!");
        return false;
    }

    s_input = new InputData();
    platform_initialize();
    return true;
}

void Input::shutdown()
{
    if (!s_input)
    {
        SE_LOG_WARN("The input has already been shut down!");
        return;
    }

    s_input->key_code_states.clear_and_shrink();
    s_input->mouse_button_states.clear_and_shrink();

    platform_shutdown();
    delete s_input;
    s_input = nullptr;
}

void Input::on_update()
{
    for (auto it : s_input->key_code_states)
    {
        it.value.was_pressed_this_frame = false;
        it.value.was_released_this_frame = false;
    }

    for (auto it : s_input->mouse_button_states)
    {
        it.value.was_pressed_this_frame = false;
        it.value.was_released_this_frame = false;
    }

    for (const OwnPtr<Event>& event_to_process : s_input->events_to_process)
    {
        const Event& generic_event = *event_to_process;
        switch (generic_event.get_type())
        {
            case EventType::KeyDown:
            {
                const auto& key_down_event = static_cast<const KeyDownEvent&>(generic_event);
                KeyState& key_state = s_input->key_code_states.get_or_add(key_down_event.get_key_code());
                if (!key_state.is_down)
                {
                    key_state.is_down = true;
                    key_state.was_pressed_this_frame = true;
                }
            }
            break;

            case EventType::KeyUp:
            {
                const auto& key_up_event = static_cast<const KeyUpEvent&>(generic_event);
                KeyState& key_state = s_input->key_code_states.get_or_add(key_up_event.get_key_code());
                if (key_state.is_down)
                {
                    key_state.is_down = false;
                    key_state.was_released_this_frame = true;
                }
            }
            break;

            case EventType::MouseButtonDown:
            {
                const auto& mouse_button_down_event = static_cast<const MouseButtonDownEvent&>(generic_event);
                MouseButtonState& mouse_button_state = s_input->mouse_button_states.get_or_add(mouse_button_down_event.get_mouse_button());
                if (!mouse_button_state.is_down)
                {
                    mouse_button_state.is_down = true;
                    mouse_button_state.was_pressed_this_frame = true;
                }
            }
            break;

            case EventType::MouseButtonUp:
            {
                const auto& mouse_button_up_event = static_cast<const MouseButtonUpEvent&>(generic_event);
                MouseButtonState& mouse_button_state = s_input->mouse_button_states.get_or_add(mouse_button_up_event.get_mouse_button());
                if (mouse_button_state.is_down)
                {
                    mouse_button_state.is_down = false;
                    mouse_button_state.was_released_this_frame = true;
                }
            }
            break;
        }
    }

    s_input->mouse_position_offset = IntVector2(0, 0);
    s_input->mouse_wheel_scroll_offset = 0.0F;
    const IntVector2 current_mouse_position = platform_get_mouse_position();

    if (s_input->last_frame_mouse_position.has_value())
    {
        IntVector2 last_position = s_input->last_frame_mouse_position.value();
        s_input->mouse_position_offset = current_mouse_position - last_position;
    }
    s_input->last_frame_mouse_position = current_mouse_position;

    s_input->events_to_process.clear();
}

void Input::on_event(const Event& in_event)
{
    switch (in_event.get_type())
    {
        case EventType::KeyDown:
        {
            const KeyDownEvent& casted_event = static_cast<const KeyDownEvent&>(in_event);
            OwnPtr<KeyDownEvent> event_to_process = create_own<KeyDownEvent>(casted_event);
            s_input->events_to_process.add(event_to_process.as<Event>());
        }
        break;

        case EventType::KeyUp:
        {
            const KeyUpEvent& casted_event = static_cast<const KeyUpEvent&>(in_event);
            OwnPtr<KeyUpEvent> event_to_process = create_own<KeyUpEvent>(casted_event);
            s_input->events_to_process.add(event_to_process.as<Event>());
        }
        break;

        case EventType::MouseButtonDown:
        {
            const MouseButtonDownEvent& casted_event = static_cast<const MouseButtonDownEvent&>(in_event);
            OwnPtr<MouseButtonDownEvent> event_to_process = create_own<MouseButtonDownEvent>(casted_event);
            s_input->events_to_process.add(event_to_process.as<Event>());
        }
        break;

        case EventType::MouseButtonUp:
        {
            const MouseButtonUpEvent& casted_event = static_cast<const MouseButtonUpEvent&>(in_event);
            OwnPtr<MouseButtonUpEvent> event_to_process = create_own<MouseButtonUpEvent>(casted_event);
            s_input->events_to_process.add(event_to_process.as<Event>());
        }
        break;
    }
}

bool Input::is_key_down(KeyCode key_code)
{
    const KeyState& key_state = s_input->key_code_states.get_or_add(key_code);
    return key_state.is_down;
}

bool Input::was_key_pressed_this_frame(KeyCode key_code)
{
    const KeyState& key_state = s_input->key_code_states.get_or_add(key_code);
    return key_state.was_pressed_this_frame;
}

bool Input::is_key_up(KeyCode key_code)
{
    const KeyState& key_state = s_input->key_code_states.get_or_add(key_code);
    return !key_state.is_down;
}

bool Input::was_key_released_this_frame(KeyCode key_code)
{
    const KeyState& key_state = s_input->key_code_states.get_or_add(key_code);
    return key_state.was_released_this_frame;
}

bool Input::is_mouse_button_down(MouseButton mouse_button)
{
    const MouseButtonState& button_state = s_input->mouse_button_states.get_or_add(mouse_button);
    return button_state.is_down;
}

bool Input::was_mouse_button_pressed_this_frame(MouseButton mouse_button)
{
    const MouseButtonState& button_state = s_input->mouse_button_states.get_or_add(mouse_button);
    return button_state.was_pressed_this_frame;
}

bool Input::is_mouse_button_up(MouseButton mouse_button)
{
    const MouseButtonState& button_state = s_input->mouse_button_states.get_or_add(mouse_button);
    return !button_state.is_down;
}

bool Input::was_mouse_button_released_this_frame(MouseButton mouse_button)
{
    const MouseButtonState& button_state = s_input->mouse_button_states.get_or_add(mouse_button);
    return button_state.was_released_this_frame;
}

float Input::get_mouse_offset_x()
{
    const float mouse_sensitivity_x = 1.0F;
    return s_input->mouse_position_offset.x * mouse_sensitivity_x;
}

float Input::get_mouse_offset_y()
{
    const float mouse_sensitivity_y = 1.0F;
    return s_input->mouse_position_offset.y * mouse_sensitivity_y;
}

float Input::get_mouse_wheel_scroll_offset()
{
    return s_input->mouse_wheel_scroll_offset;
}

void Input::clear_all_key_states()
{
    for (auto it : s_input->key_code_states)
    {
        it.value.was_pressed_this_frame = false;
        it.value.was_released_this_frame = false;

        if (it.value.is_down)
        {
            it.value.is_down = false;
            it.value.was_released_this_frame = true;
        }
    }
}

void Input::clear_all_mouse_button_states()
{
    for (auto it : s_input->mouse_button_states)
    {
        it.value.was_pressed_this_frame = false;
        it.value.was_released_this_frame = false;

        if (it.value.is_down)
        {
            it.value.is_down = false;
            it.value.was_released_this_frame = true;
        }
    }
}

void Input::clear_all_states()
{
    clear_all_key_states();
    clear_all_mouse_button_states();
}

} // namespace SE
