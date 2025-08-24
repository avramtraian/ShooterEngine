// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Core/Math/Vector.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// 3X3 MATRIX STRUCTURES. /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct Matrix3
{
public:
    union
    {
        Vector3 Rows[3];
        float M[3][3];
        float E[9];
    };

public:
    FORCEINLINE Matrix3()
        : Rows {}
    {}

    FORCEINLINE Matrix3(const Matrix3& other)
    {
        Rows[0] = other.Rows[0];
        Rows[1] = other.Rows[1];
        Rows[2] = other.Rows[2];
    }

    FORCEINLINE Matrix3(Vector3 row0, Vector3 row1, Vector3 row2)
        : Rows { row0, row1, row2 }
    {}
    
    FORCEINLINE Matrix3(
        float m00, float m01, float m02,
        float m10, float m11, float m12,
        float m20, float m21, float m22
    )
        : E { m00, m01, m02, m10, m11, m12, m20, m21, m22 }
    {}

public:
    FORCEINLINE Matrix3& operator=(const Matrix3& other)
    {
        Rows[0] = other.Rows[0];
        Rows[1] = other.Rows[1];
        Rows[2] = other.Rows[2];
        return *this;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// 4X4 MATRIX STRUCTURES. /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

enum class NDCDepthRange : uint8
{
    ZeroToOne,
    MinusOneToOne,
};

struct Matrix4
{
public:
    NODISCARD FORCEINLINE static Matrix4 Identity()
    {
        return Matrix4(
            1.0F, 0.0F, 0.0F, 0.0F,
            0.0F, 1.0F, 0.0F, 0.0F,
            0.0F, 0.0F, 1.0F, 0.0F,
            0.0F, 0.0F, 0.0F, 1.0F
        );
    }

public:
    union
    {
        Vector4 Rows[4];
        float M[4][4];
        float E[16];
    };

public:
    FORCEINLINE Matrix4()
        : Rows {}
    {}

    FORCEINLINE Matrix4(const Matrix4& other)
    {
        Rows[0] = other.Rows[0];
        Rows[1] = other.Rows[1];
        Rows[2] = other.Rows[2];
        Rows[3] = other.Rows[3];
    }

    FORCEINLINE Matrix4(Vector4 row0, Vector4 row1, Vector4 row2, Vector4 row3)
        : Rows { row0, row1, row2, row3 }
    {}
    
    FORCEINLINE Matrix4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    )
        : E { m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33 }
    {}

public:
    FORCEINLINE Matrix4& operator=(const Matrix4& other)
    {
        Rows[0] = other.Rows[0];
        Rows[1] = other.Rows[1];
        Rows[2] = other.Rows[2];
        Rows[3] = other.Rows[3];
        return *this;
    }

public:
    NODISCARD FORCEINLINE Matrix4 Inversed() const { return Matrix4::Inverse(*this); }
    NODISCARD FORCEINLINE Matrix4 Transposed() const { return Matrix4::Transpose(*this); }

public:
    NODISCARD FORCEINLINE static Matrix4 Multiply(const Matrix4& lhs, const Matrix4& rhs);
    NODISCARD FORCEINLINE static Vector4 Multiply(const Vector4& lhs, const Matrix4& rhs);
    NODISCARD FORCEINLINE static Vector4 Multiply(const Matrix4& lhs, const Vector4& rhs);

    NODISCARD FORCEINLINE static Matrix4 Inverse(const Matrix4& matrix);
    NODISCARD FORCEINLINE static Matrix4 Transpose(const Matrix4& matrix);

    NODISCARD FORCEINLINE static Matrix4 Translate(float translationX, float translationY, float translationZ);
    NODISCARD FORCEINLINE static Matrix4 Translate(Vector3 translation);

    NODISCARD FORCEINLINE static Matrix4 RotateX(float angleInRad);
    NODISCARD FORCEINLINE static Matrix4 RotateY(float angleInRad);
    NODISCARD FORCEINLINE static Matrix4 RotateZ(float angleInRad);

    NODISCARD FORCEINLINE static Matrix4 Rotate(float angleX, float angleY, float angleZ);
    NODISCARD FORCEINLINE static Matrix4 Rotate(Vector3 rotation);

    NODISCARD FORCEINLINE static Matrix4 Scale(float scaleX, float scaleY, float scaleZ);
    NODISCARD FORCEINLINE static Matrix4 Scale(Vector3 scale);

    NODISCARD FORCEINLINE static Matrix4 PerspectiveProjection(float fovInRad, float aspectRatio, float nearZ, float farZ);
    NODISCARD FORCEINLINE static Matrix4 OrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ);

    NODISCARD FORCEINLINE static Matrix4 FromCameraView(Vector3 cameraPosition, Vector3 cameraRotation);
    NODISCARD FORCEINLINE static Matrix4 LookAtView(Vector3 eyePosition, Vector3 atPosition, Vector3 upDirection);
};

NODISCARD FORCEINLINE Matrix4 operator*(const Matrix4& lhs, const Matrix4& rhs)
{
    return Matrix4::Multiply(lhs, rhs);
}

NODISCARD FORCEINLINE Vector4 operator*(const Vector4& lhs, const Matrix4& rhs)
{
    return Matrix4::Multiply(lhs, rhs);
}

NODISCARD FORCEINLINE Vector4 operator*(const Matrix4& lhs, const Vector4& rhs)
{
    return Matrix4::Multiply(lhs, rhs);
}

}
