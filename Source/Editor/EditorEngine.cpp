/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "EditorEngine.h"

namespace SE {

EditorEngine* g_editor_engine = nullptr;

bool EditorEngine::initialize()
{
    if (!Engine::initialize())
        return false;
    g_editor_engine = this;

    return g_editor_engine->create_window();
    return true;
}

void EditorEngine::shutdown()
{
    m_window_stack.clear_and_shrink();

    g_editor_engine = nullptr;
    Engine::shutdown();
}

void EditorEngine::tick()
{
    for (OwnPtr<Window>& window : m_window_stack)
        window->pump_messages();

    usize active_window_count = m_window_stack.count();
    for (auto it = m_window_stack.rbegin(); it != m_window_stack.rend(); --it) {
        OwnPtr<Window>& window = *it;
        if (window->should_close()) {
            window.release();
            m_window_stack.remove(it);
            --active_window_count;
        }
    }

    if (active_window_count == 0 || m_window_stack.first()->should_close()) {
        m_window_stack.clear_and_shrink();
        m_is_running = false;
        return;
    }

    Engine::tick();
}

Window* EditorEngine::create_window()
{
    WindowInfo window_info = {};
    window_info.start_maximized = true;
    window_info.title = "ShooterEditor (Windows - Debug) | Untitled Scene*"sv;

    m_window_stack.add(Window::instantiate());
    OwnPtr<Window>& window = m_window_stack.last();

    if (!window->initialize(window_info)) {
        m_window_stack.remove_last();
        return nullptr;
    }

    return window.get();
}

Window* EditorEngine::find_window_by_native_handle(void* native_handle)
{
    //
    // If the last window in the stack doesn't have a valid native handle it means that the
    // Window class was instantiated, but the native window object hasn't been created yet.
    // On Windows, the function used to create the native window will post messages that
    // must be processed, but its native handle is still unknown.
    //
    if (m_window_stack.last()->get_native_handle() == nullptr)
        return m_window_stack.last().get();

    for (OwnPtr<Window>& window : m_window_stack) {
        if (window->get_native_handle() == native_handle)
            return window.get();
    }

    return nullptr;
}

} // namespace SE