// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

template<typename T>
class Badge
{
    SE_MAKE_NONCOPYABLE(Badge);
    SE_MAKE_NONMOVABLE(Badge);

    friend class T;

private:
    Badge() = default;
};

}
