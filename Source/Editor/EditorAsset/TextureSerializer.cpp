/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Asset/TextureAsset.h>
#include <Core/Containers/StringBuilder.h>
#include <Core/FileSystem/FileSystem.h>
#include <Core/Log.h>
#include <EditorAsset/TextureSerializer.h>
#include <EditorEngine.h>
#include <ThirdParty/yaml-cpp/include/yaml-cpp/yaml.h>

#define STBI_ASSERT(x) SE_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include <ThirdParty/stb_image/stb_image.h>

namespace SE
{

bool TextureSerializer::serialize(AssetHandle handle)
{
    RefPtr<TextureAsset> asset = g_asset_manager->get_asset_sync<TextureAsset>(handle);
    EditorAssetMetadata& metadata = g_editor_asset_manager->get_editor_metadata(handle);

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Type" << YAML::Value << get_asset_type_string(metadata.type).characters();
    out << YAML::Key << "Handle" << YAML::Value << (u64)(metadata.handle);
    out << YAML::Key << "Filepath" << YAML::Value << asset->get_texture_filepath().characters();
    out << YAML::EndMap;
    const StringView yaml_view = StringView::create_from_utf8(out.c_str());

    const String metadata_filepath = StringBuilder::path_join({ g_editor_engine->get_project_content_directory().view(), metadata.filepath.view() });

    FileWriter asset_metadata_file_writer;
    SE_CHECK_FILE_ERROR(asset_metadata_file_writer.open(metadata_filepath));
    SE_CHECK_FILE_ERROR(asset_metadata_file_writer.write_and_close(yaml_view.byte_span()));

    return true;
}

RefPtr<Asset> TextureSerializer::deserialize(AssetMetadata& metadata)
{
    EditorAssetMetadata& editor_metadata = static_cast<EditorAssetMetadata&>(metadata);

    const String asset_metadata_filepath =
        StringBuilder::path_join({ g_editor_engine->get_project_content_directory().view(), editor_metadata.filepath.view() });

    FileReader asset_metadata_file_reader;
    String asset_metadata_file;
    SE_CHECK_FILE_ERROR(asset_metadata_file_reader.open(asset_metadata_filepath));
    SE_CHECK_FILE_ERROR(asset_metadata_file_reader.read_entire_to_string_and_close(asset_metadata_file));

    YAML::Node asset_metadata_root = YAML::Load(asset_metadata_file.characters());
    if (!asset_metadata_root)
    {
        SE_LOG_TAG_ERROR("Asset", "Invalid or corrupted asset file: '{}'!", editor_metadata.filepath);
        return {};
    }

    YAML::Node asset_type_node = asset_metadata_root["Type"];
    YAML::Node asset_handle_node = asset_metadata_root["Handle"];

    if (!asset_type_node || !asset_handle_node)
    {
        SE_LOG_TAG_ERROR("Asset", "Invalid or corrupted asset file: '{}'!", editor_metadata.filepath);
        return {};
    }

    const String asset_type_string = StringView::create_from_utf8(asset_type_node.as<std::string>().c_str());
    const AssetType asset_type = get_asset_type_from_string(asset_type_string.view());
    const AssetHandle asset_handle = AssetHandle(asset_handle_node.as<u64>());
    if (asset_type == AssetType::Unknown)
    {
        SE_LOG_TAG_ERROR("Asset", "Invalid asset type ({}) for ID '{}'!", asset_type_string, (u64)(asset_handle));
        return {};
    }

    if (asset_type != editor_metadata.type)
    {
        SE_LOG_TAG_ERROR(
            "Asset",
            "Expected asset type '{}', but found '{}'! ({})",
            get_asset_type_string(editor_metadata.type),
            get_asset_type_string(asset_type),
            editor_metadata.filepath
        );
        return {};
    }

    if (asset_handle != editor_metadata.handle)
    {
        SE_LOG_TAG_ERROR(
            "Asset", "Expected asset handle '{}', but found '{}'! ({})", (u64)(editor_metadata.handle), (u64)(asset_handle), editor_metadata.filepath
        );
        return {};
    }

    YAML::Node texture_filepath_node = asset_metadata_root["Filepath"];
    if (!texture_filepath_node)
    {
        SE_LOG_TAG_ERROR("Asset", "Invalid or corrupted asset file: '{}'!", editor_metadata.filepath);
        return {};
    }

    const String texture_filepath = StringView::create_from_utf8(texture_filepath_node.as<std::string>().c_str());
    const String absolute_texture_filepath = StringBuilder::path_join({ g_editor_engine->get_project_content_directory().view(), texture_filepath.view() });

    FileReader texture_file_reader;
    Buffer texture_file;
    SE_CHECK_FILE_ERROR(texture_file_reader.open(absolute_texture_filepath));
    SE_CHECK_FILE_ERROR(texture_file_reader.read_entire_and_close(texture_file));

    stbi_set_flip_vertically_on_load(true);

    int width, height;
    const u32 channel_count = 4;
    stbi_uc* loaded_texture_bytes = stbi_load_from_memory(texture_file.bytes(), (int)(texture_file.byte_count()), &width, &height, nullptr, channel_count);
    texture_file.release();

    const usize loaded_texture_byte_count = (usize)(width) * (usize)(height) * (usize)(channel_count);

    const u32 texture_width = (u32)(width);
    const u32 texture_height = (u32)(height);
    const u32 texture_channel_count = channel_count;

    Texture2DDescription renderer_texture_description = {};
    renderer_texture_description.width = texture_width;
    renderer_texture_description.height = texture_width;
    renderer_texture_description.format = ImageFormat::RGBA8;
    renderer_texture_description.data = ReadonlyByteSpan(loaded_texture_bytes, loaded_texture_byte_count);

    RefPtr<Texture2D> renderer_texture = Texture2D::create(renderer_texture_description);
    STBI_FREE(loaded_texture_bytes);

    RefPtr<TextureAsset> asset = create_ref<TextureAsset>(move(renderer_texture), texture_filepath);
    return asset.as<Asset>();
}

} // namespace SE
