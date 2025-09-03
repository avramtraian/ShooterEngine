// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <cstdint>

namespace SE
{

template<typename T>
struct NumericLimits
{
    NODISCARD FORCEINLINE static constexpr T Min()
    {
        static_assert(false, "You must specialize NumericLimits<T> in order to use the 'Min' function!");
        return T();
    }

    NODISCARD FORCEINLINE static constexpr T Max()
    {
        static_assert(false, "You must specialize NumericLimits<T> in order to use the 'Max' function!");
        return T();
    }
};

#define SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(PrimitiveType, MIN_VALUE, MAX_VALUE) \
    template<>                                                                            \
    struct NumericLimits<PrimitiveType>                                                   \
    {                                                                                     \
        NODISCARD FORCEINLINE static constexpr PrimitiveType Min()                        \
        {                                                                                 \
            return PrimitiveType(MIN_VALUE);                                              \
        }                                                                                 \
                                                                                          \
        NODISCARD FORCEINLINE static constexpr PrimitiveType Max()                        \
        {                                                                                 \
            return PrimitiveType(MAX_VALUE);                                              \
        }                                                                                 \
    };

SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(uint8,  0, 0xFF);
SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(uint16, 0, 0xFFFF);
SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(uint32, 0, 0xFFFFFFFF);
SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(uint64, 0, 0xFFFFFFFFFFFFFFFF);

SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(int8,  INT8_MIN, 0x7F);
SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(int16, INT16_MIN, 0x7FFF);
SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(int32, INT32_MIN, 0x7FFFFFFF);
SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(int64, INT64_MIN, 0x7FFFFFFFFFFFFFFF);

#undef SE_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE

}
