// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/Math/Random.h>
#include <random>

namespace SE
{

static std::random_device s_random_device;

static std::mt19937 s_random_generator_32(s_random_device());
static std::mt19937_64 s_random_generator_64(s_random_device());

static std::uniform_int_distribution<u32> s_uint_distribution_32;
static std::uniform_int_distribution<u64> s_uint_distribution_64;

u32 Random::uint_32()
{
    return s_uint_distribution_32(s_random_generator_32);
}

u64 Random::uint_64()
{
    return s_uint_distribution_64(s_random_generator_64);
}

u32 Random::uint_32_range(u32 range_min, u32 range_max)
{
    SE_CHECK(range_min <= range_max);
    const u32 random_value = Random::uint_32();
    return range_min + random_value % (range_max - range_min + 1);
}

u64 Random::uint_64_range(u64 range_min, u64 range_max)
{
    SE_CHECK(range_min <= range_max);
    const u64 random_value = Random::uint_64();
    return range_min + random_value % (range_max - range_min + 1);
}

float Random::float_range(float range_min, float range_max)
{
    SE_CHECK(range_min <= range_max);
    const float scalar = (float)(Random::uint_64()) / (float)(UINT64_MAX);
    return range_min + scalar * (range_max - range_min);
}

} // namespace SE
