/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Function.h>
#include <Core/Containers/Optional.h>
#include <Core/UUID.h>
#include <Engine/Scene/Scene.h>

namespace SE
{

using PFN_OnSceneHierarchySelectionChanged = Function<void(Optional<UUID> selected_entity_uuid)>;

class SceneHierarchyPanel
{
public:
    struct EntityEntry
    {
        ALWAYS_INLINE EntityEntry(Entity& in_entity, const String& in_name)
            : entity(in_entity)
            , name(in_name)
        {}

        Entity& entity;
        const String& name;
    };

public:
    bool initialize();
    void shutdown();

    void on_update(float delta_time);
    void on_render_imgui();

public:
    NODISCARD ALWAYS_INLINE Scene* get_scene_context() { return m_scene_context; }
    NODISCARD ALWAYS_INLINE const Scene* get_scene_context() const { return m_scene_context; }
    NODISCARD ALWAYS_INLINE bool has_scene_context() const { return (m_scene_context != nullptr); }

    void set_scene_context(Scene* scene_context);
    void clear_scene_context();

    ALWAYS_INLINE void add_on_selection_changed_callback(PFN_OnSceneHierarchySelectionChanged callback)
    {
        SE_ASSERT(callback.has_value());
        m_on_selection_changed_callbacks.emplace(move(callback));
    }

private:
    bool draw_entity_entry(const EntityEntry& entry);

    void set_selected_entity(UUID selected_entity_uuid);
    void clear_selected_entity();
    void dispatch_on_selection_changed_callbacks();

private:
    Scene* m_scene_context;

    Optional<UUID> m_selected_entity_uuid;
    Vector<PFN_OnSceneHierarchySelectionChanged> m_on_selection_changed_callbacks;
};

} // namespace SE
