/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/FileSystem/FileSystem.h>
#include <Core/Log.h>
#include <Core/String/StringBuilder.h>
#include <EditorAsset/EditorAssetManager.h>
#include <EditorAsset/TextureSerializer.h>
#include <EditorEngine.h>
#include <ThirdParty/yaml-cpp/include/yaml-cpp/shooteryaml.h>

namespace SE
{

EditorAssetManager* g_editor_asset_manager = nullptr;

bool EditorAssetManager::initialize()
{
    SE_ASSERT(g_editor_asset_manager == nullptr);
    g_editor_asset_manager = static_cast<EditorAssetManager*>(g_asset_manager);

    initialize_asset_registry();
    initialize_asset_serializers();

    return true;
}

void EditorAssetManager::shutdown()
{
    SE_ASSERT(g_editor_asset_manager == this);

    // NOTE: Requesting the serialization of the asset registry might not be the
    //       responsibility of the AssetManager at all. Maybe don't slow down
    //       the shutdown procedure by it?
    serialize_asset_registry();

    m_asset_serializers.clear_and_shrink();
    m_asset_registry.clear_and_shrink();

    g_editor_asset_manager = nullptr;
    // Required by the AssetManager specification.
    AssetManager::shutdown();
}

#define SE_ASSET_REGISTRY_FILENAME "AssetRegistry.se"sv

bool EditorAssetManager::initialize_asset_registry()
{
    m_asset_registry.clear();
    if (!deserialize_asset_registry())
        return false;

    SE_LOG_TAG_INFO("Asset", "The asset registry contains '{}' assets.", m_asset_registry.count());
    return true;
}

void EditorAssetManager::initialize_asset_serializers()
{
    m_asset_serializers.clear();

    m_asset_serializers.add(AssetType::Texture, create_own<TextureSerializer>().as<AssetSerializer>());
}

String EditorAssetManager::get_asset_registry_filepath()
{
    return StringBuilder::path_join({ g_editor_engine->context().get_project_root_directory().view(), SE_ASSET_REGISTRY_FILENAME });
}

bool EditorAssetManager::serialize_asset_registry()
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Assets" << YAML::BeginSeq;

    usize serialized_asset_count = 0;
    for (const auto& bucket : m_asset_registry)
    {
        const AssetHandle handle = bucket.key;
        const AssetSlot& slot = bucket.value;

        // Skip all memory only assets.
        if (slot.metadata.is_memory_only)
            continue;

        out << YAML::BeginMap;
        out << YAML::Key << "Type" << YAML::Value << get_asset_type_string(slot.metadata.type).characters();
        out << YAML::Key << "Handle" << YAML::Value << slot.metadata.handle.value();
        out << YAML::Key << "Filepath" << YAML::Value << slot.metadata.filepath.characters();
        out << YAML::EndMap;

        if (slot.metadata.state == AssetState::Ready)
        {
            m_asset_serializers.at(slot.metadata.type)->serialize(handle);
            ++serialized_asset_count;
        }
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;
    const StringView yaml_view = StringView::create_from_utf8(out.c_str());

    FileWriter asset_registry_file_writer;
    SE_CHECK_FILE_ERROR(asset_registry_file_writer.open(get_asset_registry_filepath()));
    SE_CHECK_FILE_ERROR(asset_registry_file_writer.write_and_close(yaml_view.byte_span()));

    SE_LOG_TAG_INFO("Asset", "Serialized '{}' assets.", serialized_asset_count);
    return true;
}

bool EditorAssetManager::deserialize_asset_registry()
{
    SE_ASSERT(m_asset_registry.is_empty());

    FileReader asset_registry_reader;
    SE_CHECK_FILE_ERROR(asset_registry_reader.open(get_asset_registry_filepath()));
    String asset_registry_file;
    SE_CHECK_FILE_ERROR(asset_registry_reader.read_entire_to_string_and_close(asset_registry_file));

    YAML::Node asset_registry_root = YAML::Load(asset_registry_file.characters());
    if (!asset_registry_root)
    {
        SE_LOG_TAG_ERROR("Asset", "Invalid or currupted AssetRegistry file!");
        return false;
    }

    YAML::Node asset_list = asset_registry_root["Assets"];
    if (!asset_list || !asset_list.IsSequence())
    {
        SE_LOG_TAG_ERROR("Asset", "Invalid or corrupted AssetRegistry file!");
        return false;
    }

    for (const YAML::Node& asset : asset_list)
    {
        const auto& asset_type_node = asset["Type"];
        const auto& asset_handle_node = asset["Handle"];
        const auto& asset_filepath_node = asset["Filepath"];

        if (!asset_type_node || !asset_handle_node || !asset_filepath_node)
        {
            SE_LOG_TAG_ERROR("Asset", "Invalid asset description encountered. Skipping...");
            continue;
        }

        const AssetHandle asset_handle = AssetHandle(asset_handle_node.as<UUID>());
        const String asset_filepath = StringView::create_from_utf8(asset_filepath_node.as<std::string>().c_str());

        const String asset_type_string = StringView::create_from_utf8(asset_type_node.as<std::string>().c_str());
        const AssetType asset_type = get_asset_type_from_string(asset_type_string.view());
        if (asset_type == AssetType::Unknown)
        {
            SE_LOG_TAG_ERROR("Asset", "Invalid asset type ({}) encountered for asset ID '{}'! Skipping...", asset_type_string, asset_handle.value());
            continue;
        }

        if (get_asset_type_from_file_extension(asset_filepath.path_extension()) != asset_type)
        {
            SE_LOG_TAG_ERROR(
                "Asset",
                "The filepath ({}) of the asset with handle '{}' doesn't match the asset type extension ({})! Skipping...",
                asset_filepath,
                asset_handle.value(),
                get_asset_type_file_extension(asset_type)
            );
            continue;
        }

        if (m_asset_registry.contains(asset_handle))
        {
            SE_LOG_TAG_ERROR("Asset", "The asset handle ({}) already exists in the registry! Skipping...", asset_handle.value());
            continue;
        }

        AssetSlot asset_slot;
        asset_slot.metadata.type = asset_type;
        asset_slot.metadata.state = AssetState::Unloaded;
        asset_slot.metadata.handle = asset_handle;
        asset_slot.metadata.filepath = move(asset_filepath);
        asset_slot.metadata.is_memory_only = false;

        m_asset_registry.add(asset_handle, move(asset_slot));
    }

    return true;
}

RefPtr<Asset> EditorAssetManager::get_asset_sync(AssetHandle handle)
{
    Optional<AssetSlot&> optional_asset_slot = m_asset_registry.get_if_exists(handle);
    if (!optional_asset_slot.has_value())
    {
        SE_LOG_TAG_ERROR("Asset", "Querying an invalid asset ID ({})!", handle.value());
        return {};
    }
    AssetSlot& asset_slot = *optional_asset_slot;

    // Check if the asset is already loaded.
    if (asset_slot.metadata.state == AssetState::Ready)
        return asset_slot.asset;

    // A memory-only asset must always be ready!
    SE_ASSERT(!asset_slot.metadata.is_memory_only);

    RefPtr<Asset> loaded_asset;
    loaded_asset = m_asset_serializers[asset_slot.metadata.type]->deserialize(asset_slot.metadata);
    if (!loaded_asset.is_valid())
    {
        SE_LOG_TAG_ERROR("Asset", "Failed to load asset with ID '{}'!", handle.value());
        return {};
    }

    asset_slot.asset = move(loaded_asset);
    asset_slot.metadata.state = AssetState::Ready;
    return asset_slot.asset;
}

AssetMetadata& EditorAssetManager::get_asset_metadata(AssetHandle handle)
{
    Optional<AssetSlot&> optional_asset_slot = m_asset_registry.get_if_exists(handle);
    if (!optional_asset_slot.has_value())
    {
        SE_LOG_TAG_ERROR("Asset", "Querying an invalid asset ID ({})!", handle.value());
        return m_empty_asset_slot.metadata;
    }

    return optional_asset_slot->metadata;
}

EditorAssetMetadata& EditorAssetManager::get_editor_metadata(AssetHandle handle)
{
    AssetMetadata& metadata = get_asset_metadata(handle);
    return static_cast<EditorAssetMetadata&>(metadata);
}

void EditorAssetManager::register_memory_only_asset(RefPtr<Asset> asset)
{
    const AssetHandle handle = AssetHandle::create();
    SE_DEBUG_ASSERT(!m_asset_registry.contains(handle));

    AssetSlot asset_slot;
    asset_slot.metadata.type = asset->get_type();
    asset_slot.metadata.state = AssetState::Ready;
    asset_slot.metadata.handle = handle;
    asset_slot.metadata.is_memory_only = true;
    asset_slot.asset = move(asset);

    m_asset_registry.add(handle, move(asset_slot));
}

} // namespace SE
