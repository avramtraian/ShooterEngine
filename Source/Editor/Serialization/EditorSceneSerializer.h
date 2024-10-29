/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/String/String.h>

namespace YAML
{

// YAML forward declarations.
class Node;
class Emitter;

} // namespace YAML

namespace SE
{

// Forward declarations.
class Scene;
class Entity;
class EntityComponent;
class ComponentReflectorRegistry;
struct ComponentField;

class EditorSceneSerializer
{
    SE_MAKE_NONCOPYABLE(EditorSceneSerializer);
    SE_MAKE_NONMOVABLE(EditorSceneSerializer);

public:
    ALWAYS_INLINE explicit EditorSceneSerializer(Scene& scene_context, const ComponentReflectorRegistry& component_reflector_registry_context)
        : m_scene_context(scene_context)
        , m_component_reflector_registry_context(component_reflector_registry_context)
    {}

    bool serialize(const String& filepath);
    bool deserialize(const String& filepath);

private:
    bool serialize_entity(YAML::Emitter& emitter, const Entity& entity);
    bool serialize_entity_component(YAML::Emitter& emitter, const EntityComponent& component);
    bool serialize_component_field(YAML::Emitter& emitter, const EntityComponent& component, const ComponentField& field);

    bool deserialize_entity(const YAML::Node& node);
    bool deserialize_entity_component(const YAML::Node& node, Entity& entity);
    bool deserialize_component_field(const YAML::Node& node, Entity& entity, EntityComponent& component);

private:
    Scene& m_scene_context;
    const ComponentReflectorRegistry& m_component_reflector_registry_context;
};

} // namespace SE
