/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/CoreDefines.h>

#if SE_PLATFORM_WINDOWS
    #define SE_VECTORIZED_MATH_USE_DIRECTX 1
    #include <DirectXMath.h>
#endif // SE_PLATFORM_WINDOWS

#ifndef SE_VECTORIZED_MATH_USE_DIRECTX
    #define SE_VECTORIZED_MATH_USE_DIRECTX 0
#endif // SE_VECTORIZED_MATH_USE_DIRECTX
