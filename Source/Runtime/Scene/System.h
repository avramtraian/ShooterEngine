// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class System
{
    SE_MAKE_NONCOPYABLE(System);
    SE_MAKE_NONMOVABLE(System);

public:
    System() = default;
    virtual ~System() = default;

    RUNTIME_API virtual void OnBeginPlay();
    RUNTIME_API virtual void OnEndPlay();
    RUNTIME_API virtual void OnUpdate(float deltaTime);
};

}
