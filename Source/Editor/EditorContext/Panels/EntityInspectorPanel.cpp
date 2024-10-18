/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <EditorContext/Panels/EntityInspectorPanel.h>
#include <imgui.h>

namespace SE
{

bool EntityInspectorPanel::initialize()
{
    return true;
}

void EntityInspectorPanel::shutdown()
{}

void EntityInspectorPanel::on_update(float delta_time)
{}

void EntityInspectorPanel::on_render_imgui()
{
    ImGui::Begin("EntityInspector");
    ImGui::End();
}

} // namespace SE
