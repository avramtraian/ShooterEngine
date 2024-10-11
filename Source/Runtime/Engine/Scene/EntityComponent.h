/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>
#include <Core/UUID.h>

namespace SE
{

// Forward declarations.
class Entity;
class Scene;

//
// Structure that is always passes as the first argument to any constructor of any entity component.
// Contains information about the component initialization context, such as the scene or the parent entity.
//
struct EntityComponentInitializer
{
    Entity* parent_entity { nullptr };
    Scene* scene_context { nullptr };
};

//
// Base class of any entity component, either engine-related or game-related.
// An entity component is the core logic unit that can implement game-related code.
//
class EntityComponent
{
    SE_MAKE_NONCOPYABLE(EntityComponent);
    SE_MAKE_NONMOVABLE(EntityComponent);

    friend class Entity;

public:
    NODISCARD virtual bool check_component_type_uuid(UUID component_type_uuid) const { return false; }
    NODISCARD virtual UUID get_component_type_uuid() const = 0;

    virtual ~EntityComponent() = default;

public:
    // Returns a pointer to the entity that contains the given component.
    NODISCARD ALWAYS_INLINE Entity* parent_entity() { return &m_parent_entity; }
    NODISCARD ALWAYS_INLINE const Entity* parent_entity() const { return &m_parent_entity; }

    // Returns whether or not the `on_update` callback is invoked.
    NODISCARD ALWAYS_INLINE bool is_updatable() const { return m_is_updatable; }

    SHOOTER_API void set_is_updatable(bool is_updatable);
    ALWAYS_INLINE void toggle_is_updatable() { set_is_updatable(!is_updatable()); }

protected:
    SHOOTER_API explicit EntityComponent(const EntityComponentInitializer& initializer);

    SHOOTER_API virtual void on_begin_play() {}
    SHOOTER_API virtual void on_end_play() {}
    SHOOTER_API virtual void on_update(float delta_time) {}

private:
    Entity& m_parent_entity;
    bool m_is_updatable;
};

} // namespace SE

#define SE_ENTITY_COMPONENT(type_name, parent_type_name)                                                    \
                                                                                                            \
public:                                                                                                     \
    using Super = parent_type_name;                                                                         \
    NODISCARD static UUID get_static_component_type_uuid();                                                 \
    NODISCARD ALWAYS_INLINE virtual UUID get_component_type_uuid() const override                           \
    {                                                                                                       \
        return get_static_component_type_uuid();                                                            \
    }                                                                                                       \
    NODISCARD ALWAYS_INLINE virtual bool check_component_type_uuid(UUID component_type_uuid) const override \
    {                                                                                                       \
        if (get_static_component_type_uuid() == component_type_uuid)                                        \
            return true;                                                                                    \
        return Super::check_component_type_uuid(component_type_uuid);                                       \
    }                                                                                                       \
    ALWAYS_INLINE explicit type_name(const EntityComponentInitializer& initializer)                         \
        : parent_type_name(initializer)                                                                     \
    {}                                                                                                      \
    virtual ~type_name() override = default;

#define SE_ENGINE_ENTITY_COMPONENT(type_name, parent_type_name)                                             \
                                                                                                            \
public:                                                                                                     \
    using Super = parent_type_name;                                                                         \
    NODISCARD SHOOTER_API static UUID get_static_component_type_uuid();                                     \
    NODISCARD ALWAYS_INLINE virtual UUID get_component_type_uuid() const override                           \
    {                                                                                                       \
        return get_static_component_type_uuid();                                                            \
    }                                                                                                       \
    NODISCARD ALWAYS_INLINE virtual bool check_component_type_uuid(UUID component_type_uuid) const override \
    {                                                                                                       \
        if (get_static_component_type_uuid() == component_type_uuid)                                        \
            return true;                                                                                    \
        return Super::check_component_type_uuid(component_type_uuid);                                       \
    }                                                                                                       \
    ALWAYS_INLINE explicit type_name(const EntityComponentInitializer& initializer)                         \
        : parent_type_name(initializer)                                                                     \
    {}                                                                                                      \
    virtual ~type_name() override = default;
