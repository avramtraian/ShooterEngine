/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/OwnPtr.h>
#include <Core/Containers/String.h>
#include <Core/Containers/Vector.h>
#include <Core/UUID.h>
#include <Engine/Scene/EntityComponent.h>

namespace SE
{

// Forward declarations.
class Scene;

//
// The entity class is reponsible for managing a set of components. The entity itself should't contain
// any game-related logic, and it is purely an abstraction over the component-scene architecture.
// A scene represents a list of entities, and every entity represents a list of components.
//
// Every entity is created by the scene object, and every component is created using its parent entity API.
// Observation: The entity itself might not be the one that determines its components' lifetimes. Components
// are created in the context of the scene, but entities are the objects that manage their activation/deactivation logic.
//
class Entity
{
    SE_MAKE_NONCOPYABLE(Entity);
    SE_MAKE_NONMOVABLE(Entity);

    friend class OwnPtr<Entity>;
    friend class Scene;

#if SE_CONFIGURATION_TARGET_EDITOR
    friend class EntityInspectorPanel;
    friend class SceneHierarchyPanel;
#endif // SE_CONFIGURATION_TARGET_EDITOR

public:
    // Returns the scene context where the entity exists.
    NODISCARD ALWAYS_INLINE Scene& scene_context() { return m_scene_context; }
    NODISCARD ALWAYS_INLINE const Scene& scene_context() const { return m_scene_context; }

    // Returns the globally unique identifier of the entity.
    NODISCARD ALWAYS_INLINE UUID uuid() const { return m_uuid; }

    // Returns the name of the entity in the scene.
    NODISCARD ALWAYS_INLINE const String& name() const { return m_name; }

public:
    //
    // Checks if the entity has a component of the given template parameter or a component of a type derived from it.
    //
    template<typename ComponentType>
    NODISCARD ALWAYS_INLINE bool has_component() const
    {
        const UUID component_type_uuid = ComponentType::get_static_component_type_uuid();
        return has_component(component_type_uuid);
    }

    //
    // Gets a reference to the component of the given template parameter or the component of a type derived from it.
    // If the entity doesn't have neither of the component types listed above, this function will assert.
    //
    template<typename ComponentType>
    NODISCARD ALWAYS_INLINE ComponentType& get_component()
    {
        const UUID component_type_uuid = ComponentType::get_static_component_type_uuid();
        EntityComponent* component = get_component(component_type_uuid);
        SE_ASSERT(component != nullptr);

        return static_cast<ComponentType&>(*component);
    }

    //
    // Gets a const reference to the component of the given template parameter or the component of a type derived from it.
    // If the entity doesn't have neither of the component types listed above, this function will assert.
    //
    template<typename ComponentType>
    NODISCARD ALWAYS_INLINE const ComponentType& get_component() const
    {
        const UUID component_type_uuid = ComponentType::get_static_component_type_uuid();
        const EntityComponent* component = get_component(component_type_uuid);
        SE_ASSERT(component != nullptr);

        return static_cast<const ComponentType&>(*component);
    }

    //
    // Adds a component of the given template parameter and returns a reference to it.
    // If the entity already has a component of the given template parameter or a component of a type derived from it, this
    // function will issue an assert.
    //
    template<typename ComponentType, typename... Args>
    ALWAYS_INLINE ComponentType& add_component(Args&&... args)
    {
        SE_ASSERT(!has_component<ComponentType>());

        EntityComponentInitializer initializer = {};
        initializer.parent_entity = this;
        initializer.scene_context = &m_scene_context;

        ComponentType* component = new ComponentType(initializer, forward<Args>(args)...);
        add_component(static_cast<EntityComponent*>(component));
        return *component;
    }

public:
    SHOOTER_API void set_name(String entity_name);

    NODISCARD ALWAYS_INLINE const Vector<EntityComponent*>& get_components() const { return m_components; }

private:
    //
    // Function that is called when the scene begins a new play session or the entity has just been created
    // in the scene.
    //
    // Invokes the `on_begin_play` callback for each component the entity has.
    // Also runs the logic required for an entity to be ready to start the play session.
    //
    void on_begin_play();

    //
    // Function that is called when the scene ends the current play session or the entity has just been destroyed
    // from the scene.
    //
    // Invokes the `on_end_play` callback for each component the entity has.
    // Also runs the logic required for an entity to correctly end the playing session.
    //
    void on_end_play();

    //
    // Functions that is called each frame when the scene is in a play session.
    //
    // Invokes the `on_update` callback for each component the entity has.
    // Also runs the logic required for an entity to be correctly updated.
    //
    void on_update(float delta_time);

private:
    Entity(Scene& in_scene_context, UUID entity_uuid);
    ~Entity();

    SHOOTER_API bool has_component(UUID component_type_uuid) const;
    SHOOTER_API EntityComponent* get_component(UUID component_type_uuid);
    SHOOTER_API const EntityComponent* get_component(UUID component_type_uuid) const;
    SHOOTER_API void add_component(EntityComponent* component);

private:
    Scene& m_scene_context;
    UUID m_uuid;
    String m_name;

    Vector<EntityComponent*> m_components;
};

} // namespace SE
