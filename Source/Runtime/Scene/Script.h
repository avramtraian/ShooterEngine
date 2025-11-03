// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class Script
{
    SE_MAKE_NONCOPYABLE(Script);
    SE_MAKE_NONMOVABLE(Script);

public:
    Script() = default;
    virtual ~Script() = default;

    RUNTIME_API virtual void OnBeginPlay();
    RUNTIME_API virtual void OnEndPlay();
    RUNTIME_API virtual void OnUpdate(float deltaTime);
};

}
