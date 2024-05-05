/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreTypes.h"

namespace SE
{

class Math
{
public:
    template<typename T>
    NODISCARD ALWAYS_INLINE static T min(T x, T y)
    {
        return (x < y) ? x : y;
    }

    template<typename T>
    NODISCARD ALWAYS_INLINE static T max(T x, T y)
    {
        return (x > y) ? x : y;
    }
};

} // namespace SE