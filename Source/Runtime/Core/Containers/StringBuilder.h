/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/String.h"

#include <initializer_list>

namespace SE
{

class StringBuilder
{
public:
    NODISCARD SHOOTER_API static String join(std::initializer_list<StringView> views_list);

public: // Filepath API.
    // If a given path doesn't start with a path delimitation character, this function will insert one.
    NODISCARD SHOOTER_API static String path_join(std::initializer_list<StringView> paths_list);
};

} // namespace SE