/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/Random.h>
#include <Core/UUID.h>

namespace SE
{

UUID UUID::create()
{
    // Generate a new 64-bit unsigned integer value.
    const u64 uuid_value = Random::UInt64();
    return UUID(uuid_value);
}

} // namespace SE
