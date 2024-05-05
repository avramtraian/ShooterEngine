/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreTypes.h"
#include "Core/EngineAPI.h"
#include "Core/Memory/Buffer.h"

namespace SE {

class FileSystem {
public:
    SHOOTER_API NODISCARD static bool file_exists(const String& filepath);

    // If the provided filepath is not valid, this function will return an empty optional.
    SHOOTER_API NODISCARD static Optional<bool> is_directory(const String& filepath);

    // If the provided filepath is not valid or it doesn't represent a file, this function
    // will return an empty optional.
    SHOOTER_API NODISCARD static Optional<usize> get_file_size(const String& filepath);
};

} // namespace SE