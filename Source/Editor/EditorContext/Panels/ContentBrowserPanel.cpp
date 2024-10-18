/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <EditorContext/Panels/ContentBrowserPanel.h>
#include <imgui.h>

namespace SE
{

bool ContentBrowserPanel::initialize()
{
    return true;
}

void ContentBrowserPanel::shutdown()
{}

void ContentBrowserPanel::on_update(float delta_time)
{}

void ContentBrowserPanel::on_render_imgui()
{
    ImGui::Begin("ContentBrowser");
    ImGui::End();
}

} // namespace SE
