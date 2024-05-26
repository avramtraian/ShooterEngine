/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/OwnPtr.h"
#include "Core/EngineAPI.h"
#include "Renderer/Renderer2D.h"

namespace SE
{

class WorldRenderer
{
public:
    SE_MAKE_NONCOPYABLE(WorldRenderer);
    SE_MAKE_NONMOVABLE(WorldRenderer);
    WorldRenderer() = default;

public:
    SHOOTER_API bool initialize(u32 width, u32 height);
    SHOOTER_API void shutdown();

    SHOOTER_API bool render();

private:
    OwnPtr<Renderer2D> m_renderer_2d;
};

} // namespace SE
