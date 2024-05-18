/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Asset/AssetManager.h"
#include "Asset/AssetSerializer.h"
#include "Core/Containers/HashMap.h"
#include "Core/Containers/OwnPtr.h"
#include "Core/Containers/RefPtr.h"
#include "Core/Containers/String.h"

namespace SE
{

class EditorAssetMetadata : public AssetMetadata
{
public:
    String filepath;
    bool is_memory_only;
};

class EditorAssetManager : public AssetManager
{
public:
    virtual bool initialize() override;
    virtual void shutdown() override;

    virtual RefPtr<Asset> get_asset_sync(AssetHandle handle) override;
    virtual AssetMetadata& get_asset_metadata(AssetHandle handle) override;

    EditorAssetMetadata& get_editor_metadata(AssetHandle handle);

    template<typename T, typename... Args>
    ALWAYS_INLINE RefPtr<T> create_memory_only_asset(Args&&... args)
    {
        RefPtr<T> asset = make_ref<T>(forward<Args>(args)...);
        register_memory_only_asset(asset.as<Asset>());
        return asset;
    }

private:
    bool initialize_asset_registry();
    void initialize_asset_serializers();

    String get_asset_registry_filepath();
    bool serialize_asset_registry();
    bool deserialize_asset_registry();

    void register_memory_only_asset(RefPtr<Asset> asset);

private:
    struct AssetSlot
    {
        EditorAssetMetadata metadata;
        RefPtr<Asset> asset;
    };

    HashMap<AssetHandle, AssetSlot> m_asset_registry;
    AssetSlot m_empty_asset_slot;

    HashMap<AssetType, OwnPtr<AssetSerializer>> m_asset_serializers;
};

extern EditorAssetManager* g_editor_asset_manager;

} // namespace SE
