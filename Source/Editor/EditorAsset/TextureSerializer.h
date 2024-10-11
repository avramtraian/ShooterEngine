/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Asset/AssetSerializer.h>
#include <Core/Containers/RefPtr.h>
#include <EditorAsset/EditorAssetManager.h>

namespace SE
{

class TextureSerializer : public AssetSerializer
{
public:
    virtual bool serialize(AssetHandle handle) override;
    virtual RefPtr<Asset> deserialize(AssetMetadata& metadata) override;
};

} // namespace SE
