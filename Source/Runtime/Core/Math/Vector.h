// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

struct Vector2
{
public:
    float X;
    float Y;

public:
    FORCEINLINE Vector2()
        : X(0)
        , Y(0)
    {}

    FORCEINLINE Vector2(const Vector2& other)
        : X(other.X)
        , Y(other.Y)
    {}

    FORCEINLINE Vector2(float x, float y)
        : X(x)
        , Y(y)
    {}
};

struct Vector2i
{
public:
    int32 X;
    int32 Y;

public:
    FORCEINLINE Vector2i()
        : X(0)
        , Y(0)
    {}

    FORCEINLINE Vector2i(const Vector2i& other)
        : X(other.X)
        , Y(other.Y)
    {}

    FORCEINLINE Vector2i(int32 x, int32 y)
        : X(x)
        , Y(y)
    {}
};

struct Vector2u
{
public:
    uint32_t X;
    uint32_t Y;

public:
    FORCEINLINE Vector2u()
        : X(0)
        , Y(0)
    {}

    FORCEINLINE Vector2u(const Vector2u& other)
        : X(other.X)
        , Y(other.Y)
    {}

    FORCEINLINE Vector2u(uint32 x, uint32 y)
        : X(x)
        , Y(y)
    {}
};

}
