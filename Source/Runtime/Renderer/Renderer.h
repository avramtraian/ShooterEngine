/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Engine/Application/Window.h"
#include "Renderer/RenderingContext.h"

namespace SE
{

class Renderer
{
public:
    SHOOTER_API static bool initialize();
    SHOOTER_API static void shutdown();

    SHOOTER_API static bool create_context_for_window(Window* window);
    SHOOTER_API static RenderingContext* get_rendering_context_for_window(Window* window);
    
    SHOOTER_API static RenderingContext* get_active_context();
    SHOOTER_API static void set_active_context(RenderingContext* context);
};

} // namespace SE