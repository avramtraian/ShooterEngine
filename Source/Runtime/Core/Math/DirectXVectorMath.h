/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Math/Matrix4.h>
#include <Core/Math/Vector4.h>

#include <DirectXMath.h>

namespace SE
{

using VectorRegister = DirectX::XMVECTOR;
using MatrixRegister = DirectX::XMMATRIX;

//
// Wrappers around the DirectXMath loading/storing functions.
// Utility functions designed to simply the usage of the DirectXMath library.
//
// clang-format off
NODISCARD ALWAYS_INLINE VectorRegister vector_load_float(float in_float) { return DirectX::XMLoadFloat(&in_float); }
NODISCARD ALWAYS_INLINE VectorRegister vector_load_float_4(const Vector4& float_4) { return DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&float_4); }
NODISCARD ALWAYS_INLINE MatrixRegister vector_load_float_4x4(const Matrix4& float_4x4) { return DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4 *)&float_4x4); }
ALWAYS_INLINE void vector_store_float(float& dst_float, const VectorRegister& vector) { DirectX::XMStoreFloat(&dst_float, vector); }
ALWAYS_INLINE void vector_store_float_4(Vector4& dst_float_4, const VectorRegister& vector) { DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)&dst_float_4, vector); }
ALWAYS_INLINE void vector_store_float_4x4(Matrix4& dst_float_4x4, const MatrixRegister& matrix) { DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&dst_float_4x4, matrix); }
// clang-format on

ALWAYS_INLINE void vector_matrix_multiply(Matrix4& destination, const Matrix4& left, const Matrix4& right)
{
    MatrixRegister left_register = vector_load_float_4x4(left);
    MatrixRegister right_register = vector_load_float_4x4(right);
    MatrixRegister result_register = DirectX::XMMatrixMultiply(left_register, right_register);
    vector_store_float_4x4(destination, result_register);
}

ALWAYS_INLINE void vector_matrix_transpose(Matrix4& destination, const Matrix4& matrix)
{
    MatrixRegister matrix_register = vector_load_float_4x4(matrix);
    MatrixRegister result_register = DirectX::XMMatrixTranspose(matrix_register);
    vector_store_float_4x4(destination, result_register);
}

ALWAYS_INLINE void vector_matrix_inverse_and_determinant(const Matrix4& matrix, Matrix4& destination, float& out_determinant)
{
    MatrixRegister matrix_register = vector_load_float_4x4(matrix);
    VectorRegister determinant_register;
    MatrixRegister result_register = DirectX::XMMatrixInverse(&determinant_register, matrix_register);

    vector_store_float(out_determinant, determinant_register);
    // TODO: Ensure that the determinant is non-zero, and thus the matrix is actually invertible, by asserting.
    vector_store_float_4x4(destination, result_register);
}

ALWAYS_INLINE void vector_matrix_inverse(Matrix4& destination, const Matrix4& matrix)
{
#if SE_CONFIGURATION_SHIPPING

    // This will not provide any checks that the matrix is actually invertible.
    // Instead, the DirectX documentation states that an infinite matrix will be returned.
    // Checking if invertible is actually pretty cheap, as calculating the determinant is required
    // in order to compute the inverse matrix. The only cost is loading/storing the values from/in
    // the registers, which adds a few instructions.

    MatrixRegister matrix_register = vector_load_float_4x4(matrix);
    MatrixRegister result_register = DirectX::XMMatrixInverse(nullptr, matrix_register);
    vector_store_float_4x4(destination, result_register);

#else

    float determinant;
    vector_matrix_inverse_and_determinant(matrix, destination, determinant);

#endif // SE_CONFIGURATION_SHIPPING
}

ALWAYS_INLINE float vector_matrix_determinant(const Matrix4& matrix)
{
    MatrixRegister matrix_register = vector_load_float_4x4(matrix);
    VectorRegister determinant_register = DirectX::XMMatrixDeterminant(matrix_register);
    float determinant;
    vector_store_float(determinant, determinant_register);
    return determinant;
}

ALWAYS_INLINE void vector_transform_vector(Vector4& destination, const Vector4& vector, const Matrix4& matrix)
{
    VectorRegister vector_register = vector_load_float_4(vector);
    MatrixRegister matrix_register = vector_load_float_4x4(matrix);
    VectorRegister result_register = DirectX::XMVector4Transform(vector_register, matrix_register);
    vector_store_float_4(destination, result_register);
}

ALWAYS_INLINE void VectorMatrixTranslate(Matrix4& destination, float offset_x, float offset_y, float offset_z)
{
    MatrixRegister result_register = DirectX::XMMatrixTranslation(offset_x, offset_y, offset_z);
    vector_store_float_4x4(destination, result_register);
}

ALWAYS_INLINE void vector_matrix_scale(Matrix4& destination, float scale_x, float scale_y, float scale_z)
{
    MatrixRegister result_register = DirectX::XMMatrixScaling(scale_x, scale_y, scale_z);
    vector_store_float_4x4(destination, result_register);
}

ALWAYS_INLINE void vector_matrix_rotate_x(Matrix4& destination, float angle)
{
    MatrixRegister result_register = DirectX::XMMatrixRotationX(angle);
    vector_store_float_4x4(destination, result_register);
}

ALWAYS_INLINE void vector_matrix_rotate_y(Matrix4& destination, float angle)
{
    MatrixRegister result_register = DirectX::XMMatrixRotationY(angle);
    vector_store_float_4x4(destination, result_register);
}

ALWAYS_INLINE void vector_matrix_rotate_z(Matrix4& destination, float angle)
{
    MatrixRegister result_register = DirectX::XMMatrixRotationZ(angle);
    vector_store_float_4x4(destination, result_register);
}

} // namespace SE
