/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/CoreTypes.h>
#include <Engine/Application/Window.h>

namespace SE
{

SHOOTER_API extern class Engine* g_engine;

class Engine
{
    SE_MAKE_NONCOPYABLE(Engine);
    SE_MAKE_NONMOVABLE(Engine);

public:
    template<typename T>
    ALWAYS_INLINE static void instantiate()
    {
        SE_ASSERT(g_engine == nullptr);
        g_engine = new T();
    }

    SHOOTER_API static void destroy();

public:
    SHOOTER_API virtual bool initialize();
    SHOOTER_API virtual void shutdown();

    SHOOTER_API virtual void update();
    SHOOTER_API virtual void exit();

    ALWAYS_INLINE bool is_running() const { return m_is_running; }

public:
    virtual String get_engine_root_directory() const = 0;

protected:
    Engine() = default;
    virtual ~Engine() = default;

protected:
    bool m_is_running = false;
};

} // namespace SE
