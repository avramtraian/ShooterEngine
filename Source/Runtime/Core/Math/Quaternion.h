/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>

namespace SE
{
namespace Internal
{

template<typename T>
struct Quaternion
{
public:
    /** These values represent the values in the standard quaternion notation. */
    T a, b, c, d;
};

} // namespace Internal

using Quaternion = Internal::Quaternion<float>;
using Quaterniond = Internal::Quaternion<double>;

} // namespace SE
