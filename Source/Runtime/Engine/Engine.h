// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class Engine
{
    SE_MAKE_SINGLETON_CLASS(Engine);

public:
    RUNTIME_API virtual bool Initialize();
    RUNTIME_API virtual void Shutdown();

    virtual void Execute() = 0;
};

RUNTIME_API extern Engine* g_Engine;

}
