// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Math/Random.h>
#include <Runtime/Core/UUID.h>

namespace SE
{

UUID UUID::Generate()
{
    const uint64 randomValue = Random::UInt64();
    return UUID(randomValue);
}

}
