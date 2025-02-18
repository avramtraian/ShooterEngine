/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Runtime/Core/CoreDefines.h>

#if SE_PLATFORM_WIN64
    #define SE_VECTORIZED_MATH_USE_DIRECTX 1
    #include <DirectXMath.h>
#endif // SE_PLATFORM_WIN64

#ifndef SE_VECTORIZED_MATH_USE_DIRECTX
    #define SE_VECTORIZED_MATH_USE_DIRECTX 0
#endif // SE_VECTORIZED_MATH_USE_DIRECTX
