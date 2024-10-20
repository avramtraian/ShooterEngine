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
{
    if (has_scene_context() && m_selected_entity_uuid.has_value())
    {
        if (m_scene_context->get_entity_from_uuid(m_selected_entity_uuid.value()) == nullptr)
        {
            // The entity has been removed from the scene by an external actor.
            clear_selected_entity();
        }
    }
}

void SceneHierarchyPanel::on_render_imgui()
{
    ImGui::Begin("SceneHierarchy");

    if (has_scene_context())
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

        bool is_any_entry_clicked = false;
        for (const EntityEntry& entry : ordered_entity_entries)
        {
            const bool entry_is_clicked = draw_entity_entry(entry);
            is_any_entry_clicked = is_any_entry_clicked || entry_is_clicked;
        }

        if (!is_any_entry_clicked && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
        {
            // If no entity entry was hovered but the mouse was clicked the user wants to clear
            // the selection context.
            clear_selected_entity();
        }
    }

    ImGui::End();
}

void SceneHierarchyPanel::set_scene_context(Scene* scene_context)
{
    SE_ASSERT(scene_context != nullptr);
    m_scene_context = scene_context;
}

void SceneHierarchyPanel::clear_scene_context()
{
    m_scene_context = nullptr;
}

bool SceneHierarchyPanel::draw_entity_entry(const EntityEntry& entry)
{
    bool entry_is_clicked = false;
    ImGui::PushID(reinterpret_cast<const void*>(entry.entity.uuid().value()));

    ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
    if (entry.entity.uuid() == m_selected_entity_uuid.value_or(UUID::invalid()))
        tree_node_flags |= ImGuiTreeNodeFlags_Selected;

    bool is_tree_node_opened = ImGui::TreeNodeEx(entry.name.characters(), tree_node_flags);

    if (ImGui::IsItemClicked())
    {
        const UUID entity_uuid = entry.entity.uuid();
        set_selected_entity(entity_uuid);
        entry_is_clicked = true;
    }

    if (ImGui::BeginPopupContextItem())
    {
        ImGui::SeparatorText("Entity Options");
        if (ImGui::MenuItem("Remove Entity"))
        {
            // TODO: Actually remove the entity from the scene.
            // This function is not yet implemented as there is no API for removing entities from a scene.
            clear_selected_entity();
        }
        ImGui::EndPopup();
    }

    if (is_tree_node_opened)
        ImGui::TreePop();

    ImGui::PopID();
    return entry_is_clicked;
}

void SceneHierarchyPanel::set_selected_entity(UUID selected_entity_uuid)
{
    SE_ASSERT(selected_entity_uuid != UUID::invalid());
    m_selected_entity_uuid = selected_entity_uuid;
    dispatch_on_selection_changed_callbacks();
}

void SceneHierarchyPanel::clear_selected_entity()
{
    m_selected_entity_uuid.clear();
    dispatch_on_selection_changed_callbacks();
}

void SceneHierarchyPanel::dispatch_on_selection_changed_callbacks()
{
    for (PFN_OnSceneHierarchySelectionChanged& callback : m_on_selection_changed_callbacks)
        callback(m_selected_entity_uuid);
}

} // namespace SE
