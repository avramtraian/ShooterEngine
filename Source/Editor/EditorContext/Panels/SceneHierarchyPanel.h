/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Engine/Scene/Scene.h>

namespace SE
{

class SceneHierarchyPanel
{
public:
    bool initialize();
    void shutdown();

    void on_update(float delta_time);
    void on_render_imgui();

public:
    NODISCARD ALWAYS_INLINE Scene* get_scene_context() { return m_scene_context; }
    NODISCARD ALWAYS_INLINE const Scene* get_scene_context() const { return m_scene_context; }

    void set_scene_context(Scene* scene_context);

    void clear_scene_context();

private:
    struct EntityEntry
    {
        ALWAYS_INLINE EntityEntry(Entity& in_entity, const String& in_name)
            : entity(in_entity)
            , name(in_name)
        {}

        Entity& entity;
        const String& name;
    };

    void render_entity_entry(const EntityEntry& entry);

private:
    Scene* m_scene_context;
};

} // namespace SE
