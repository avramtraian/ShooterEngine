/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Optional.h>
#include <Core/CoreTypes.h>
#include <Core/UUID.h>

namespace SE
{

// Forward declarations.
class ComponentReflectorRegistry;
class Entity;
class EntityComponent;
class Scene;

class EntityInspectorPanel
{
public:
    bool initialize();
    void shutdown();

    void on_update(float delta_time);
    void on_render_imgui();

public:
    NODISCARD ALWAYS_INLINE Optional<UUID> get_entity_uuid_context() { return m_entity_uuid_context; }
    NODISCARD ALWAYS_INLINE bool has_entity_uuid_context() const { return m_entity_uuid_context.has_value(); }
    void set_entity_uuid_context(UUID entity_uuid_context);
    void clear_entity_uuid_context();

    NODISCARD ALWAYS_INLINE Scene* get_scene_context() const { return m_scene_context; }
    NODISCARD ALWAYS_INLINE bool has_scene_context() const { return (m_scene_context != nullptr); }
    void set_scene_context(Scene* scene_context);
    void clear_scene_context();

    NODISCARD ALWAYS_INLINE ComponentReflectorRegistry* get_component_reflector_registry_context() const { return m_component_reflector_registry_context; }
    NODISCARD ALWAYS_INLINE bool has_component_reflector_registry_context() const { return (m_component_reflector_registry_context != nullptr); }
    void set_component_reflector_registry_context(ComponentReflectorRegistry* component_reflector_registry_context);
    void clear_component_reflector_registry_context();

private:
    void draw_entity_name(Entity& entity_context);
    void draw_entity_uuid(UUID uuid);
    void draw_add_component(Entity& entity_context);
    void draw_component(EntityComponent* component);

private:
    Optional<UUID> m_entity_uuid_context;
    Scene* m_scene_context { nullptr };
    ComponentReflectorRegistry* m_component_reflector_registry_context { nullptr };
};

} // namespace SE
