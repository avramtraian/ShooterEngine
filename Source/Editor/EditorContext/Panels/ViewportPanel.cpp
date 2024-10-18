/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <EditorContext/Panels/ViewportPanel.h>
#include <imgui.h>

namespace SE
{

bool ViewportPanel::initialize()
{
    return true;
}

void ViewportPanel::shutdown()
{
    m_on_viewport_resized_callbacks.clear_and_shrink();
    clear_scene_framebuffer_context();
}

void ViewportPanel::on_update(float delta_time)
{
    if (m_is_viewport_size_dirty)
    {
        for (auto& callback : m_on_viewport_resized_callbacks)
            callback(m_viewport_width, m_viewport_height);

        m_is_viewport_size_dirty = false;
    }
}

void ViewportPanel::on_render_imgui()
{
    ImGui::Begin("Viewport");
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 { 0, 0 });
    
    const ImVec2 viewport_size = ImGui::GetContentRegionAvail();
    const u32 viewport_width = static_cast<u32>(viewport_size.x);
    const u32 viewport_height = static_cast<u32>(viewport_size.y);

    if (m_viewport_width != viewport_width)
    {
        m_viewport_width = viewport_width;
        m_is_viewport_size_dirty = true;
    }

    if (m_viewport_height != viewport_height)
    {
        m_viewport_height = viewport_height;
        m_is_viewport_size_dirty = true;
    }

    if (m_scene_framebuffer_context.is_valid())
    {
        // Render the first attachment of the framebuffer.
        void* framebuffer_attachment_view = m_scene_framebuffer_context->get_attachment_image_view(0);
        ImGui::Image(framebuffer_attachment_view, viewport_size);
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

} // namespace SE
