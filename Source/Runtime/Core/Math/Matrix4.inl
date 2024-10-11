/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Math/Matrix4.h>
#include <Core/Math/VectorMath.h>

namespace SE
{

namespace Internal
{

template<>
NODISCARD ALWAYS_INLINE Matrix4<float> Matrix4<float>::transpose(const Matrix4& matrix)
{
    Matrix4 result;
    vector_matrix_transpose(result, matrix);
    return result;
}

template<>
NODISCARD ALWAYS_INLINE Matrix4<float> Matrix4<float>::inverse_and_determinant(const Matrix4& matrix, float& out_determinant)
{
    Matrix4 result;
    vector_matrix_inverse_and_determinant(matrix, result, out_determinant);
    return result;
}

template<>
NODISCARD ALWAYS_INLINE Matrix4<float> Matrix4<float>::inverse(const Matrix4& matrix)
{
    Matrix4 result;
    vector_matrix_inverse(result, matrix);
    return result;
}

template<>
NODISCARD ALWAYS_INLINE float Matrix4<float>::determinant(const Matrix4& matrix)
{
    return vector_matrix_determinant(matrix);
}

template<>
NODISCARD ALWAYS_INLINE Matrix4<float> Matrix4<float>::translate(float offset_x, float offset_y, float offset_z)
{
    Matrix4 result;
    VectorMatrixTranslate(result, offset_x, offset_y, offset_z);
    return result;
}

template<>
NODISCARD ALWAYS_INLINE Matrix4<float> Matrix4<float>::rotate_x(float angle)
{
    Matrix4 result;
    vector_matrix_rotate_x(result, angle);
    return result;
}

template<>
NODISCARD ALWAYS_INLINE Matrix4<float> Matrix4<float>::rotate_y(float angle)
{
    Matrix4 result;
    vector_matrix_rotate_y(result, angle);
    return result;
}

template<>
NODISCARD ALWAYS_INLINE Matrix4<float> Matrix4<float>::rotate_z(float angle)
{
    Matrix4 result;
    vector_matrix_rotate_z(result, angle);
    return result;
}

template<>
NODISCARD ALWAYS_INLINE Matrix4<float> Matrix4<float>::scale(float scale_x, float scale_y, float scale_z)
{
    Matrix4 result;
    vector_matrix_scale(result, scale_x, scale_y, scale_z);
    return result;
}

template<>
NODISCARD ALWAYS_INLINE Matrix4<float> Matrix4<float>::operator*(const Matrix4& right) const
{
    Matrix4 result;
    vector_matrix_multiply(result, *this, right);
    return result;
}

template<>
NODISCARD ALWAYS_INLINE Matrix4<float>& Matrix4<float>::operator*=(const Matrix4& right)
{
    vector_matrix_multiply(*this, *this, right);
    return *this;
}

} // namespace Internal

NODISCARD ALWAYS_INLINE Vector4 operator*(const Vector4& vector, const Matrix4& matrix)
{
    Vector4 result;
    vector_transform_vector(result, vector, matrix);
    return result;
}

} // namespace SE
