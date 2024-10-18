/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

namespace SE
{

class EntityInspectorPanel
{
public:
    bool initialize();
    void shutdown();

    void on_update(float delta_time);
    void on_render_imgui();
};

} // namespace SE
