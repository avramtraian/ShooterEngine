/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Asset/AssetManager.h>

namespace SE
{

AssetManager* g_asset_manager = nullptr;

void AssetManager::shutdown()
{
    SE_ASSERT(g_asset_manager == this);
    delete g_asset_manager;
    g_asset_manager = nullptr;
}

} // namespace SE
