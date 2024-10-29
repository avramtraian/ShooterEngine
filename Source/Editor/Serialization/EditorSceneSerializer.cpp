/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/FileSystem/FileSystem.h>
#include <Core/Log.h>
#include <Engine/Scene/Reflection/ComponentReflectorRegistry.h>
#include <Engine/Scene/Scene.h>
#include <Serialization/EditorSceneSerializer.h>
#include <yaml-cpp/shooteryaml.h>

namespace SE
{

ALWAYS_INLINE static YAML::Emitter& operator<<(YAML::Emitter& out, const ComponentFieldType& value)
{
    out << get_component_field_type_as_raw_string(value);
    return out;
}

bool EditorSceneSerializer::serialize(const String& filepath)
{
    Vector<const Entity*> entities_sorted_by_uuid;
    entities_sorted_by_uuid.set_fixed_capacity(m_scene_context.get_entity_count());

    m_scene_context.for_each_entity(
        [&](const Entity* entity, UUID entity_uuid) -> IterationDecision
        {
            entities_sorted_by_uuid.add(entity);
            return IterationDecision::Continue;
        }
    );

    entities_sorted_by_uuid.sort(
        [](const Entity* lhs, const Entity* rhs) -> ComparisonResult
        {
            if (lhs->uuid().value() < rhs->uuid().value())
                return ComparisonResult::Less;
            return ComparisonResult::Greater;
        }
    );

    YAML::Emitter out;
    out << YAML::BeginMap; // Root map.

    out << YAML::Key << "UUID" << YAML::Value << UUID::invalid();
    out << YAML::Key << "Name" << YAML::Value << "Unnamed Scene";

    out << YAML::Key << "Entities" << YAML::BeginSeq;

    SE_LOG_TAG_INFO("Editor", "Serializing '{}' entities.", entities_sorted_by_uuid.count());
    for (const Entity* entity : entities_sorted_by_uuid)
    {
        if (!serialize_entity(out, *entity))
            return false;
    }

    out << YAML::EndSeq; // Entities.
    out << YAML::EndMap; // Root.
    const StringView yaml_string_view = StringView::unsafe_create_from_utf8(out.c_str(), out.size());

    FileWriter scene_file_writer;
    SE_CHECK_FILE_ERROR(scene_file_writer.open(filepath));
    SE_CHECK_FILE_ERROR(scene_file_writer.write_and_close(yaml_string_view.byte_span()));

    SE_LOG_TAG_INFO("Editor", "Serialized scene to filepath '{}'.", filepath);
    return true;
}

bool EditorSceneSerializer::serialize_entity(YAML::Emitter& emitter, const Entity& entity)
{
    emitter << YAML::BeginMap; // Entity map.
    emitter << YAML::Key << "UUID" << YAML::Value << entity.uuid();
    emitter << YAML::Key << "Name" << YAML::Value << entity.name();
    emitter << YAML::Key << "Components" << YAML::BeginSeq;

    for (const EntityComponent* component : entity.get_components())
    {
        if (!serialize_entity_component(emitter, *component))
            return false;
    }

    emitter << YAML::EndSeq; // Components.
    emitter << YAML::EndMap; // Entity map.
    return true;
}

bool EditorSceneSerializer::serialize_entity_component(YAML::Emitter& emitter, const EntityComponent& component)
{
    emitter << YAML::BeginMap; // Component map.
    const ComponentReflector& reflector = m_component_reflector_registry_context.get_reflector(component.get_component_type_uuid());

    emitter << YAML::Key << "Name" << reflector.name;
    emitter << YAML::Key << "TypeUUID" << component.get_component_type_uuid();
    emitter << YAML::Key << "Fields" << YAML::BeginSeq;

    for (const ComponentField& field : reflector.fields)
    {
        if (!serialize_component_field(emitter, component, field))
            return false;
    }

    emitter << YAML::EndSeq; // Fields.
    emitter << YAML::EndMap; // Componenet map.
    return true;
}

bool EditorSceneSerializer::serialize_component_field(YAML::Emitter& emitter, const EntityComponent& component, const ComponentField& field)
{
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "Name" << YAML::Value << field.name;
    emitter << YAML::Key << "Type" << YAML::Value << field.type_stack.first();
    emitter << YAML::Key << "Value" << YAML::Value;

    const u8* component_bytes = reinterpret_cast<const u8*>(&component);

    switch (field.type_stack.first())
    {
#define CASE_STATEMENT(field_type, type) \
    case ComponentFieldType::field_type: emitter << field.get_value<type>(&component); break

        CASE_STATEMENT(Int8, i8);
        CASE_STATEMENT(Int16, i16);
        CASE_STATEMENT(Int32, i32);
        CASE_STATEMENT(Int64, i64);
        CASE_STATEMENT(Float32, float);
        CASE_STATEMENT(Float64, double);
        CASE_STATEMENT(Boolean, bool);
        CASE_STATEMENT(Vector2, Vector2);
        CASE_STATEMENT(Vector3, Vector3);
        CASE_STATEMENT(Vector4, Vector4);
        CASE_STATEMENT(Color3, Color3);
        CASE_STATEMENT(Color4, Color4);
        CASE_STATEMENT(String, String);

#undef CASE_STATEMENT
    }

    emitter << YAML::EndMap;
    return true;
}

bool EditorSceneSerializer::deserialize(const String& filepath)
{
    FileReader scene_file_reader;
    SE_CHECK_FILE_ERROR(scene_file_reader.open(filepath));
    String scene_file;
    SE_CHECK_FILE_ERROR(scene_file_reader.read_entire_to_string_and_close(scene_file));

    YAML::Node scene = YAML::Load(scene_file.characters());
    if (!scene)
    {
        SE_LOG_TAG_ERROR("Editor", "Failed to load the root YAML node from scene file '{}'!", filepath);
        return false;
    }

    const UUID scene_uuid = scene["UUID"].as<UUID>();
    const String scene_name = scene["Name"].as<String>();
    SE_LOG_TAG_TRACE("Editor", "Deserializing scene '{}' (UUID: {})", scene_name, scene_uuid);

    YAML::Node entities = scene["Entities"];
    if (!entities || !entities.IsSequence())
    {
        SE_LOG_TAG_ERROR("Editor", "Scene file is corrupted! No 'Entities' node found!");
        return false;
    }

    for (const YAML::Node& entity_node : entities)
    {
        if (!deserialize_entity(entity_node))
            return false;
    }

    return true;
}

bool EditorSceneSerializer::deserialize_entity(const YAML::Node& node)
{
    const UUID entity_uuid = node["UUID"].as<UUID>();
    const String entity_name = node["Name"].as<String>();

    SE_ASSERT(entity_uuid.is_valid());
    Entity* entity = m_scene_context.create_entity_with_uuid(entity_uuid);
    entity->set_name(entity_name);

    YAML::Node components = node["Components"];
    if (!components || !components.IsSequence())
    {
        SE_LOG_TAG_ERROR("Editor", "Scene file is corrupted! No 'Components' node found for entity '{}'!", entity_uuid);
        return false;
    }

    for (const YAML::Node& component_node : components)
    {
        if (!deserialize_entity_component(component_node, *entity))
            return false;
    }

    return true;
}

bool EditorSceneSerializer::deserialize_entity_component(const YAML::Node& node, Entity& entity)
{
    const String name = node["Name"].as<String>();
    const UUID type_uuid = node["TypeUUID"].as<UUID>();

    YAML::Node fields = node["Fields"];
    if (!fields || !fields.IsSequence())
    {
        SE_LOG_TAG_ERROR("Editor", "Scene file is corrupted! No 'Fields' node found for component '{}', entity '{}'!", name, entity.uuid());
        return false;
    }

    const ComponentReflector& reflector = m_component_reflector_registry_context.get_reflector(type_uuid);
    void* component_memory = ::operator new(reflector.structure_byte_count);

    EntityComponentInitializer initializer = {};
    initializer.parent_entity = &entity;
    initializer.scene_context = &m_scene_context;
    reflector.instantiate_function(component_memory, initializer);
    EntityComponent* component = static_cast<EntityComponent*>(component_memory);
    entity.add_component(component);

    for (const YAML::Node& field_node : fields)
    {
        if (!deserialize_component_field(field_node, entity, *component))
            return false;
    }

    return true;
}

bool EditorSceneSerializer::deserialize_component_field(const YAML::Node& node, Entity& entity, EntityComponent& component)
{
    const Vector<ComponentField>& reflector_fields = m_component_reflector_registry_context.get_reflector(component.get_component_type_uuid()).fields;

    const String name = node["Name"].as<String>();
    const ComponentFieldType type = get_component_field_type_from_string(node["Type"].as<String>().view());
    const YAML::Node value = node["Value"];

    for (const ComponentField& reflector_field : reflector_fields)
    {
        if (reflector_field.name == name && reflector_field.type_stack.first() == type)
        {
            switch (type)
            {
#define CASE_STATEMENT(field_type, type) \
    case ComponentFieldType::field_type: reflector_field.get_value<type>(&component) = value.as<type>(); break

                CASE_STATEMENT(UInt8, u8);
                CASE_STATEMENT(UInt16, u16);
                CASE_STATEMENT(UInt32, u32);
                CASE_STATEMENT(UInt64, u64);
                CASE_STATEMENT(Int8, i8);
                CASE_STATEMENT(Int16, i16);
                CASE_STATEMENT(Int32, i32);
                CASE_STATEMENT(Int64, i64);
                CASE_STATEMENT(Float32, float);
                CASE_STATEMENT(Float64, double);
                CASE_STATEMENT(Boolean, bool);
                CASE_STATEMENT(Vector2, Vector2);
                CASE_STATEMENT(Vector3, Vector3);
                CASE_STATEMENT(Vector4, Vector4);
                CASE_STATEMENT(Color3, Color3);
                CASE_STATEMENT(Color4, Color4);
                CASE_STATEMENT(String, String);

#undef CASE_STATEMENT
            }
            break;
        }
    }

    return true;
}

} // namespace SE
