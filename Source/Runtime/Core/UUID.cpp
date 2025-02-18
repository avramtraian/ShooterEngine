// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Math/Random.h>
#include <Runtime/Core/UUID.h>

namespace SE
{

UUID UUID::create()
{
    // Generate a new 64-bit unsigned integer value.
    const u64 uuid_value = Random::uint_64();
    return UUID(uuid_value);
}

} // namespace SE
