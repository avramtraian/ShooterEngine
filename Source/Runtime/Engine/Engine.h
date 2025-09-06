// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class Engine
{
    SE_MAKE_SINGLETON_CLASS(Engine);

public:
    SHOOTER_API virtual bool Initialize();
    SHOOTER_API virtual void Shutdown();

    virtual void Execute() = 0;
};

SHOOTER_API extern Engine* g_Engine;

}
