/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/OwnPtr.h"

namespace SE
{

class RendererInterface
{
public:
    static OwnPtr<RendererInterface> create();

public:
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
};

} // namespace SE