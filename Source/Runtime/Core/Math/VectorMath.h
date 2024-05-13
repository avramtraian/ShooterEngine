/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Math/Matrix4.h"

namespace SE
{

ALWAYS_INLINE void vector_matrix_multiply(Matrix4& destination, const Matrix4& left, const Matrix4& right);

ALWAYS_INLINE void vector_matrix_transpose(Matrix4& destination, const Matrix4& matrix);

ALWAYS_INLINE void vector_matrix_inverse_and_determinant(const Matrix4& matrix, Matrix4& destination, float& out_determinant);

ALWAYS_INLINE void vector_matrix_inverse(Matrix4& destination, const Matrix4& matrix);

NODISCARD ALWAYS_INLINE float vector_matrix_determinant(const Matrix4& matrix);

ALWAYS_INLINE void vector_transform_vector(Vector4& destination, const Vector4& vector, const Matrix4& matrix);

ALWAYS_INLINE void VectorMatrixTranslate(Matrix4& destination, float offset_x, float offset_y, float offset_z);

ALWAYS_INLINE void vector_matrix_scale(Matrix4& destination, float scale_x, float scale_y, float scale_z);

ALWAYS_INLINE void vector_matrix_rotate_x(Matrix4& destination, float angle);
ALWAYS_INLINE void vector_matrix_rotate_y(Matrix4& destination, float angle);
ALWAYS_INLINE void vector_matrix_rotate_z(Matrix4& destination, float angle);

} // namespace SE

#if SE_PLATFORM_WINDOWS
    #include "Core/Math/DirectXVectorMath.h"
#endif // SE_PLATFORM_WINDOWS