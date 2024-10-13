/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>

namespace SE
{

enum class ImageFormat : u32
{
    Unknown = 0,
    RGBA8,
    BGRA8,
};

enum class ImageFilteringMode : u32
{
    Linear,
    Nearest,
};

enum class ImageAddressMode : u32
{
    ClampToEdge,
    Repeat,
    MirrorRepeat,
};

} // namespace SE
