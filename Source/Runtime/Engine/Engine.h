/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreTypes.h"
#include "Core/EngineAPI.h"

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