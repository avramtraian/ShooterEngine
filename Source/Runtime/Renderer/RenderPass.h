/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Framebuffer.h"
#include "Renderer/Pipeline.h"

namespace SE
{

struct RenderPassInfo
{
    RefPtr<Pipeline>    pipeline;
    RefPtr<Framebuffer> target_framebuffer;
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