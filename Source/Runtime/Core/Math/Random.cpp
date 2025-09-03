// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/Math/Random.h>

#include <random>

namespace SE
{

static std::random_device s_random_device;

static std::mt19937 s_random_generator_32(s_random_device());
static std::mt19937_64 s_random_generator_64(s_random_device());

static std::uniform_int_distribution<uint32> s_uint_distribution_32;
static std::uniform_int_distribution<uint64> s_uint_distribution_64;

uint32 Random::UInt32()
{
    return s_uint_distribution_32(s_random_generator_32);
}

uint64 Random::UInt64()
{
    return s_uint_distribution_64(s_random_generator_64);
}

uint32 Random::UInt32Range(uint32 range_min, uint32 range_max)
{
    SE_ASSERT(range_min <= range_max);
    const uint32 random_value = Random::UInt32();
    return range_min + random_value % (range_max - range_min + 1);
}

uint64 Random::UInt64Range(uint64 range_min, uint64 range_max)
{
    SE_ASSERT(range_min <= range_max);
    const uint64 random_value = Random::UInt64();
    return range_min + random_value % (range_max - range_min + 1);
}

float Random::FloatRange(float range_min, float range_max)
{
    SE_ASSERT(range_min <= range_max);
    const float scalar = (float)(Random::UInt64()) / (float)(UINT64_MAX);
    return range_min + scalar * (range_max - range_min);
}

}
