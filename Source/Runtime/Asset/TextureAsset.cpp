/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Asset/TextureAsset.h"

namespace SE
{

TextureAsset::TextureAsset(String texture_filepath)
    : Asset(AssetType::Texture)
    , m_texture_filepath(move(texture_filepath))
{
}

} // namespace SE
