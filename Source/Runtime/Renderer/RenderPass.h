/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Framebuffer.h"
#include "Renderer/Pipeline.h"

namespace SE
{

struct RenderPassTarget
{
    RefPtr<Framebuffer> framebuffer;
    bool clear = true;
    // Only used when the 'clear' flag is set to true.
    Color4 clear_color = { 0, 0, 0, 255 };
};

struct RenderPassInfo
{
    RefPtr<Pipeline> pipeline;
    RenderPassTarget target;
};

class RenderPass : public RefCounted
{
public:
    RenderPass() = default;
    virtual ~RenderPass() override = default;

    static RefPtr<RenderPass> create(const RenderPassInfo& info);

public:
};

} // namespace SE