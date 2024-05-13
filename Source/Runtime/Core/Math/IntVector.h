/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreDefines.h"
#include "Core/CoreTypes.h"

namespace SE
{
namespace Internal
{

template<typename T>
struct IntVector2
{
public:
    ALWAYS_INLINE IntVector2()
        : x(T(0))
        , y(T(0))
    {}

public:
    T x;
    T y;
};

} // namespace Internal

using Vector2i = Internal::IntVector2<i32>;
using Vector2u = Internal::IntVector2<u32>;

} // namespace SE