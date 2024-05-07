/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/OwnPtr.h"

namespace SE
{

struct RenderingContextInfo
{
    class Window* window;
};

class RenderingContext
{
public:
    SE_MAKE_NONCOPYABLE(RenderingContext);
    SE_MAKE_NONMOVABLE(RenderingContext);
    RenderingContext() = default;
    virtual ~RenderingContext() = default;

    static OwnPtr<RenderingContext> create(const RenderingContextInfo& info);

public:
};

} // namespace SE