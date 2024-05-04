/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreDefines.h"
#include "Core/Platform/Windows/WindowsHeadersConditional.h"

#if !SE_PLATFORM_WINDOWS
    #error Trying to include the Windows headers, but they are not available on the current platform!
#endif // !SE_PLATFORM_WINDOWS