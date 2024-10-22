/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Math/Matrix.h>
#include <Core/Math/Vectorized.h>

namespace SE::Math
{

template<typename T>
NODISCARD ALWAYS_INLINE Matrix4<T> operator*(const Matrix4<T>& lhs, const Matrix4<T>& rhs)
{
    Matrix4<T> result;

#if SE_VECTORIZED_MATH_USE_DIRECTX
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmloadfloat4x4
    const DirectX::XMMATRIX lhs_register = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&lhs));
    const DirectX::XMMATRIX rhs_register = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&rhs));
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixmultiply
    const DirectX::XMMATRIX result_register = DirectX::XMMatrixMultiply(lhs_register, rhs_register);
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmstorefloat4x4
    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), result_register);
#endif // SE_VECTORIZED_MATH_USE_DIRECTX

    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T> operator*(Vector4<T> vector, const Matrix4<T>& matrix)
{
    Vector4<T> result;

#if SE_VECTORIZED_MATH_USE_DIRECTX
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmloadfloat4
    const DirectX::XMVECTOR vector_register = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&vector));
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmloadfloat4x4
    const DirectX::XMMATRIX matrix_register = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&matrix));
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmvector4transform
    const DirectX::XMVECTOR result_register = DirectX::XMVector4Transform(vector_register, matrix_register);
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmstorefloat4
    DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result), result_register);
#endif // SE_VECTORIZED_MATH_USE_DIRECTX

    return result;
}

template<typename T>
NODISCARD ALWAYS_INLINE Vector4<T> operator*(const Matrix4<T>& matrix, Vector4<T> vector)
{
    Vector4<T> result;

#if SE_VECTORIZED_MATH_USE_DIRECTX
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmloadfloat4x4
    const DirectX::XMMATRIX matrix_register = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&matrix));
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmloadfloat4
    const DirectX::XMVECTOR vector_register = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&vector));
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmvector4transform
    const DirectX::XMVECTOR result_register = DirectX::XMVector4Transform(matrix_register, vector_register);
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmstorefloat4
    DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result), result_register);
#endif // SE_VECTORIZED_MATH_USE_DIRECTX

    return result;
}

template<typename T>
Matrix4<T> Matrix4<T>::transpose(const Matrix4& matrix)
{
    Matrix4<T> result;

#if SE_VECTORIZED_MATH_USE_DIRECTX
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmloadfloat4x4
    const DirectX::XMMATRIX matrix_register = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&matrix));
    const DirectX::XMMATRIX result_register = DirectX::XMMatrixTranspose(matrix_register);
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmstorefloat4x4
    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), result_register);
#endif // SE_VECTORIZED_MATH_USE_DIRECTX

    return result;
}

template<typename T>
Matrix4<T> Matrix4<T>::inverse(const Matrix4<T>& matrix)
{
    Matrix4<T> result;

#if SE_VECTORIZED_MATH_USE_DIRECTX
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmloadfloat4x4
    const DirectX::XMMATRIX matrix_register = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&matrix));
    const DirectX::XMMATRIX result_register = DirectX::XMMatrixInverse(nullptr, matrix_register);
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmstorefloat4x4
    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), result_register);
#endif // SE_VECTORIZED_MATH_USE_DIRECTX

    return result;
}

template<typename T>
Matrix4<T> Matrix4<T>::translate(Vector3<T> translation)
{
    Matrix4<T> result;

#if SE_VECTORIZED_MATH_USE_DIRECTX
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixtranslation
    const DirectX::XMMATRIX result_register = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmstorefloat4x4
    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), result_register);
#endif // SE_VECTORIZED_MATH_USE_DIRECTX

    return result;
}

template<typename T>
Matrix4<T> Matrix4<T>::rotate(Vector3<T> rotation)
{
    Matrix4<T> result;

#if SE_VECTORIZED_MATH_USE_DIRECTX
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixrotationrollpitchyaw
    const DirectX::XMMATRIX result_register = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmstorefloat4x4
    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), result_register);
#endif // SE_VECTORIZED_MATH_USE_DIRECTX

    return result;
}

template<typename T>
Matrix4<T> Matrix4<T>::scale(Vector3<T> scale)
{
    Matrix4<T> result;

#if SE_VECTORIZED_MATH_USE_DIRECTX
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixscaling
    const DirectX::XMMATRIX result_register = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmstorefloat4x4
    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), result_register);
#endif // SE_VECTORIZED_MATH_USE_DIRECTX

    return result;
}

template<typename T>
Matrix4<T> Matrix4<T>::perspective(T vertical_fov, T aspect_ratio, T clip_near, T clip_far)
{
    Matrix4<T> result;

#if SE_VECTORIZED_MATH_USE_DIRECTX
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixperspectivefovlh
    const DirectX::XMMATRIX result_register = DirectX::XMMatrixPerspectiveFovLH(vertical_fov, aspect_ratio, clip_near, clip_far);
    // https://learn.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmstorefloat4x4
    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), result_register);
#endif // SE_VECTORIZED_MATH_USE_DIRECTX

    return result;
}

} // namespace SE::Math
