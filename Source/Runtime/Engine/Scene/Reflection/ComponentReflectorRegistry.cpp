/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/Components/CameraComponent.h>
#include <Engine/Scene/Components/SpriteRendererComponent.h>
#include <Engine/Scene/Components/TransformComponent.h>
#include <Engine/Scene/EntityComponent.h>
#include <Engine/Scene/Reflection/ComponentReflectorRegistry.h>

namespace SE
{

bool ComponentReflectorRegistry::initialize()
{
    EntityComponentInitializer default_component_initializer = {};
    default_component_initializer.parent_entity = nullptr;
    default_component_initializer.scene_context = nullptr;

#define SE_REGISTER_ENGINE_COMPONENT(component_type)                                                                      \
    {                                                                                                                     \
        ComponentReflector& reflector = allocate_reflector(component_type::get_static_component_type_uuid());             \
        component_type::on_register(reflector);                                                                           \
        reflector.default_component_object_buffer.allocate_new(sizeof(component_type));                                   \
        reflector.instantiate_function(reflector.default_component_object_buffer.bytes(), default_component_initializer); \
    }

    SE_REGISTER_ENGINE_COMPONENT(CameraComponent);
    SE_REGISTER_ENGINE_COMPONENT(SpriteRendererComponent);
    SE_REGISTER_ENGINE_COMPONENT(TransformComponent);

#undef SE_REGISTER_ENGINE_COMPONENT

    return true;
}

void ComponentReflectorRegistry::shutdown()
{
    for (auto it : m_registry)
    {
        ComponentReflector& reflector = it.value;
        EntityComponent* default_component_object = reinterpret_cast<EntityComponent*>(reflector.default_component_object_buffer.bytes());
        default_component_object->~EntityComponent();
    }

    m_registry.clear_and_shrink();
}

ComponentReflector& ComponentReflectorRegistry::allocate_reflector(UUID component_type_uuid)
{
    SE_ASSERT(!m_registry.contains(component_type_uuid));
    m_registry.emplace(component_type_uuid);
    return m_registry.at(component_type_uuid);
}

const ComponentReflector& ComponentReflectorRegistry::get_reflector(UUID component_type_uuid) const
{
    SE_ASSERT(component_type_uuid.is_valid());
    SE_ASSERT(m_registry.contains(component_type_uuid));
    return m_registry.at(component_type_uuid);
}

} // namespace SE
