// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Core/Math/Matrix.h>

#if SE_PLATFORM_WIN64
    #define SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    #include <DirectXMath.h>
#endif // SE_PLATFORM_WIN64

namespace SE
{

Matrix4 Matrix4::Multiply(const Matrix4& lhs, const Matrix4& rhs)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixMultiply(XMLoadFloat4x4((const XMFLOAT4X4*)&lhs), XMLoadFloat4x4((const XMFLOAT4X4*)&rhs)));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Vector4 Matrix4::Multiply(const Vector4& lhs, const Matrix4& rhs)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Vector4 result;
    XMStoreFloat4((XMFLOAT4*)&result, XMVector4Transform(XMLoadFloat4((const XMFLOAT4*)&lhs), XMLoadFloat4x4((const XMFLOAT4X4*)&rhs)));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Vector4 Matrix4::Multiply(const Matrix4& lhs, const Vector4& rhs)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Vector4 result;
    XMStoreFloat4((XMFLOAT4*)&result, XMVector4Transform(XMLoadFloat4((const XMFLOAT4*)&rhs), XMMatrixTranspose(XMLoadFloat4x4((const XMFLOAT4X4*)&lhs))));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::Inverse(const Matrix4& matrix)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixInverse(nullptr, XMLoadFloat4x4((const XMFLOAT4X4*)&matrix)));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::Transpose(const Matrix4& matrix)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixTranspose(XMLoadFloat4x4((const XMFLOAT4X4*)&matrix)));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::Translate(float translationX, float translationY, float translationZ)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixTranslation(translationX, translationY, translationZ));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::Translate(Vector3 translation)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixTranslation(translation.X, translation.Y, translation.Z));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::RotateX(float angleInRad)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixRotationX(angleInRad));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::RotateY(float angleInRad)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixRotationY(angleInRad));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::RotateZ(float angleInRad)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixRotationZ(angleInRad));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::Rotate(float angleX, float angleY, float angleZ)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixRotationRollPitchYaw(angleX, angleY, angleZ));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::Rotate(Vector3 rotation)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixRotationRollPitchYaw(rotation.X, rotation.Y, rotation.Z));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::Scale(float scaleX, float scaleY, float scaleZ)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixScaling(scaleX, scaleY, scaleZ));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::Scale(Vector3 scale)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixScaling(scale.X, scale.Y, scale.Z));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::PerspectiveProjection(float fovInRad, float aspectRatio, float nearZ, float farZ)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixPerspectiveFovLH(fovInRad, aspectRatio, nearZ, farZ));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::OrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4((XMFLOAT4X4*)&result, XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ));
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

Matrix4 Matrix4::FromCameraView(Vector3 cameraPosition, Vector3 cameraRotation)
{
    const Matrix4 invTranslate = Matrix4::Translate(-cameraPosition.X, -cameraPosition.Y, -cameraPosition.Z);
    const Matrix4 invRotate = Matrix4::Rotate(-cameraRotation.X, -cameraRotation.Y, -cameraRotation.Z);
    return invTranslate * invRotate;
}

Matrix4 Matrix4::LookAtView(Vector3 eyePosition, Vector3 atPosition, Vector3 upDirection)
{
#ifdef SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
    using namespace DirectX;
    Matrix4 result;
    XMStoreFloat4x4(
        (XMFLOAT4X4*)&result,
        XMMatrixLookAtLH(
            XMLoadFloat3((const XMFLOAT3*)&eyePosition),
            XMLoadFloat3((const XMFLOAT3*)&atPosition),
            XMLoadFloat3((const XMFLOAT3*)&upDirection)
        )
    );
    return result;
#else
    static_assert(false);
#endif // SE_MATRIX_TRANSORMATIONS_USE_DIRECTX
}

}
