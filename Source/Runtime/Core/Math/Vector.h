// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// TWO-COMPONENT VECTOR STRUCTURES. ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct Vector2
{
public:
    float X;
    float Y;

public:
    ALWAYS_INLINE Vector2()
        : X(0)
        , Y(0)
    {}

    ALWAYS_INLINE Vector2(const Vector2& other)
        : X(other.X)
        , Y(other.Y)
    {}

    ALWAYS_INLINE Vector2(float x, float y)
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
    ALWAYS_INLINE Vector2i()
        : X(0)
        , Y(0)
    {}

    ALWAYS_INLINE Vector2i(const Vector2i& other)
        : X(other.X)
        , Y(other.Y)
    {}

    ALWAYS_INLINE Vector2i(int32 x, int32 y)
        : X(x)
        , Y(y)
    {}
};

struct Vector2u
{
public:
    uint32 X;
    uint32 Y;

public:
    ALWAYS_INLINE Vector2u()
        : X(0)
        , Y(0)
    {}

    ALWAYS_INLINE Vector2u(const Vector2u& other)
        : X(other.X)
        , Y(other.Y)
    {}

    ALWAYS_INLINE Vector2u(uint32 x, uint32 y)
        : X(x)
        , Y(y)
    {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// THREE-COMPONENT VECTOR STRUCTURES. ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct Vector3
{
public:
    union
    {
        struct
        {
            float X;
            float Y;
        };
        Vector2 XY;
    };
    float Z;

public:
    ALWAYS_INLINE Vector3()
        : X(0.0F)
        , Y(0.0F)
        , Z(0.0F)
    {}

    ALWAYS_INLINE Vector3(const Vector3& other)
        : X(other.X)
        , Y(other.Y)
        , Z(other.Z)
    {}

    ALWAYS_INLINE Vector3(float x, float y, float z)
        : X(x)
        , Y(y)
        , Z(z)
    {}

public:
    ALWAYS_INLINE Vector3& operator=(const Vector3& other)
    {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
        return *this;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// FOUR-COMPONENT VECTOR STRUCTURES. ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct Vector4
{
public:
    union
    {
        struct
        {
            union
            {
                struct
                {
                    float X;
                    float Y;
                };
                Vector2 XY;
            };
            float Z;
        };
        Vector3 XYZ;
    };
    float W;

public:
    ALWAYS_INLINE Vector4()
        : X(0.0F)
        , Y(0.0F)
        , Z(0.0F)
        , W(0.0F)
    {}

    ALWAYS_INLINE Vector4(const Vector4& other)
        : X(other.X)
        , Y(other.Y)
        , Z(other.Z)
        , W(other.W)
    {}

    ALWAYS_INLINE Vector4(float x, float y, float z, float w)
        : X(x)
        , Y(y)
        , Z(z)
        , W(w)
    {}

public:
    ALWAYS_INLINE Vector4& operator=(const Vector4& other)
    {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
        W = other.W;
        return *this;
    }
};

} // namespace SE
