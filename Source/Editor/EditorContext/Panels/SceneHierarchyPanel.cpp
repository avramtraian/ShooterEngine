/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/Vector.h>
#include <Core/UUID.h>
#include <EditorContext/Panels/SceneHierarchyPanel.h>
#include <EditorEngine.h>
#include <Engine/Scene/Entity.h>
#include <imgui.h>

namespace SE
{

bool SceneHierarchyPanel::initialize()
{
    return true;
}

void SceneHierarchyPanel::shutdown()
{}

void SceneHierarchyPanel::on_update(float delta_time)
{}

void SceneHierarchyPanel::on_render_imgui()
{
    ImGui::Begin("SceneHierarchy");

    if (m_scene_context)
    {
        Vector<EntityEntry> ordered_entity_entries;
        ordered_entity_entries.set_fixed_capacity(m_scene_context->get_entity_count());

        m_scene_context->for_each_entity(
            [&](Entity* entity, UUID entity_uuid)
            {
                ordered_entity_entries.emplace(*entity, entity->name());
                return IterationDecision::Continue;
            }
        );

        // TODO: Actually sort the entries by name before rendering them.
        // ordered_entity_entries.sort();

        for (const EntityEntry& entry : ordered_entity_entries)
            render_entity_entry(entry);
    }

    ImGui::End();
}

void SceneHierarchyPanel::set_scene_context(Scene* scene_context)
{
    m_scene_context = scene_context;
}

void SceneHierarchyPanel::clear_scene_context()
{}

void SceneHierarchyPanel::render_entity_entry(const EntityEntry& entry)
{
    ImGui::Text(entry.name.characters());
}

} // namespace SE
