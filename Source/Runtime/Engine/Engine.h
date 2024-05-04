/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreTypes.h"
#include "Core/EngineAPI.h"
#include "Engine/Application/Window.h"

namespace SE {

class Engine {
    SE_MAKE_NONCOPYABLE(Engine);
    SE_MAKE_NONMOVABLE(Engine);

public:
    template<typename T>
    ALWAYS_INLINE static bool instantiate();

    SHOOTER_API static void destroy();

public:
    SHOOTER_API virtual bool initialize();
    SHOOTER_API virtual void shutdown();
    virtual ~Engine() = default;

    SHOOTER_API virtual void tick();
    ALWAYS_INLINE bool is_running() const { return m_is_running; }

    //
    // Creates a native window, invoking the platform layer.
    // The default Engine class implements this function as empty and doesn't require the classes
    // derived from it to override it. Thus, depending on the platform and build configuration, this
    // function might not do anything.
    // 
    // If the window creation is successful, this function returns the window instance. If the window
    // creation fails or the function is not overridden by the Engine implementation, this function returns nullptr.
    //
    SHOOTER_API virtual Window* create_window() { return nullptr; }

    //
    // Gets a pointer to the window that has the corresponding native handle.
    // If the native handle is invalid or doesn't exist, this function returns nullptr. If the Engine implementation
    // doesn't override this function, it will always return nullptr, as there are no windows.
    //
    SHOOTER_API virtual Window* find_window_by_native_handle(void* native_handle) { return nullptr; }

protected:
    Engine() = default;

protected:
    bool m_is_running = false;
};

SHOOTER_API extern Engine* g_engine;

template<typename T>
bool Engine::instantiate()
{
    if (g_engine)
        return false;
    g_engine = new T();
    return (g_engine != nullptr);
}

} // namespace SE