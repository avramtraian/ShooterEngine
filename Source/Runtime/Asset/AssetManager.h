/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Asset/Asset.h"
#include "Core/API.h"

namespace SE
{

class AssetManager
{
public:
    template<typename T>
    ALWAYS_INLINE static void instantiate();

public:
    virtual bool initialize() = 0;
    SHOOTER_API virtual void shutdown();

    virtual RefPtr<Asset> get_asset_sync(AssetHandle handle) = 0;
    virtual AssetMetadata& get_asset_metadata(AssetHandle handle) = 0;

    template<typename T>
    ALWAYS_INLINE RefPtr<T> get_asset_sync(AssetHandle handle)
    {
        RefPtr<Asset> asset = get_asset_sync(handle);
        if (!asset.is_valid())
            return {};

        SE_ASSERT(asset->get_type() == T::get_static_type());
        return asset.as<T>();
    }
};

SHOOTER_API extern AssetManager* g_asset_manager;

template<typename T>
void AssetManager::instantiate()
{
    SE_ASSERT(g_asset_manager == nullptr);
    g_asset_manager = new T();
}

} // namespace SE
