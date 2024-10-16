/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>

namespace SE
{

template<typename T>
class Badge
{
public:
    SE_MAKE_NONCOPYABLE(Badge);
    SE_MAKE_NONMOVABLE(Badge);

    friend typename T;

private:
    Badge() = default;
};

} // namespace SE
