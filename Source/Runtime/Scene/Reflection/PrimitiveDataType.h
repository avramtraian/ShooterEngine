// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Core/Math/Vector.h>

namespace SE
{

enum class PrimitiveDataType : uint8
{
    Unknown = 0,

#define SE_ENUMERATE_PRIMITIVE_DATA_TYPES(x)    \
    x(Int8,     int8)                           \
    x(Int16,    int16)                          \
    x(Int32,    int32)                          \
    x(Int64,    int64)                          \
    x(UInt8,    uint8)                          \
    x(UInt16,   uint16)                         \
    x(UInt32,   uint32)                         \
    x(UInt64,   uint64)                         \
    x(Float32,  Float32)                        \
    x(Float64,  Float64)                        \
    x(Bool8,    bool)                           \
    x(Vector2,  Vector2)                        \
    x(Vector3,  Vector3)                        \
    x(Vector4,  Vector4)

#define _SE_ENUM_MEMBER(Enum, Type) Enum,
    SE_ENUMERATE_PRIMITIVE_DATA_TYPES(_SE_ENUM_MEMBER)
#undef _SE_ENUM_MEMBER
};

template<typename T>
struct PrimitiveDataTypeFinder
{
    NODISCARD FORCEINLINE static constexpr PrimitiveDataType GetDataType()
    {
        static_assert(false, "You must specialize PrimitiveDataTypeFinder<T> in order to use this type!");
        return PrimitiveDataType::Unknown;
    }
};

#define _SE_DECLARE_PRIMITIVE_DATA_TYPE_FINDER(Enum, Type)                      \
    template<>                                                                  \
    struct PrimitiveDataTypeFinder<Type>                                        \
    {                                                                           \
        NODISCARD FORCEINLINE static constexpr PrimitiveDataType GetDataType()  \
        {                                                                       \
            return PrimitiveDataType::Enum;                                     \
        }                                                                       \
    };
    SE_ENUMERATE_PRIMITIVE_DATA_TYPES(_SE_DECLARE_PRIMITIVE_DATA_TYPE_FINDER)
#undef _SE_DECLARE_PRIMITIVE_DATA_TYPE_FINDER

}
