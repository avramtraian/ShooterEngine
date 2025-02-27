// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class Engine
{
public:
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void execute() = 0;

    SHOOTER_API virtual bool initialize_core_systems();
    SHOOTER_API virtual void shutdown_core_systems();

public:
    Engine() = default;
    virtual ~Engine() = default;

    SE_MAKE_NONCOPYABLE(Engine);
    SE_MAKE_NONMOVABLE(Engine);
};

SHOOTER_API extern Engine* g_engine;

}
