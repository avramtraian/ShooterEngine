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

    ENGINE_API virtual void OnBeginPlay();
    ENGINE_API virtual void OnEndPlay();
    ENGINE_API virtual void OnUpdate(float deltaTime);
};

}
