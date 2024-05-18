/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Asset/Asset.h"
#include "Core/Math/Random.h"

namespace SE
{

AssetHandle AssetHandle::create()
{
    AssetHandle result;
    result.m_handle_value = Random::UInt64();
    return result;
}

String get_asset_type_string(AssetType asset_type)
{
    switch (asset_type)
    {
        case AssetType::Unknown: return "Unknown"sv;
        case AssetType::Texture: return "Texture"sv;
    }

    SE_LOG_TAG_ERROR("Asset"sv, "Invalid AssetType!"sv);
    return {};
}

AssetType get_asset_type_from_string(StringView asset_type_string)
{
    if (asset_type_string == "Unknown"sv) return AssetType::Unknown;
    if (asset_type_string == "Texture"sv) return AssetType::Texture;

    SE_LOG_TAG_ERROR("Asset"sv, "Invalid AssetType!"sv);
    return AssetType::Unknown;
}

String get_asset_type_file_extension(AssetType asset_type)
{
    switch (asset_type)
    {
        case AssetType::Texture: return "stexture"sv;
    }

    SE_LOG_TAG_ERROR("Asset"sv, "Invalid AssetType!"sv);
    return {};
}

AssetType get_asset_type_from_file_extension(StringView file_extension)
{
    if (file_extension == "stexture"sv) return AssetType::Texture;

    SE_LOG_TAG_ERROR("Asset"sv, "Invalid asset type file extension!"sv);
    return AssetType::Unknown;
}

} // namespace SE
