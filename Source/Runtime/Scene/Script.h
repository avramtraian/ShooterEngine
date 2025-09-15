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

    ENGINE_API virtual void OnBeginPlay();
    ENGINE_API virtual void OnEndPlay();
    ENGINE_API virtual void OnUpdate(float deltaTime);
};

}
