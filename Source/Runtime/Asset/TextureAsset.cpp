/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Asset/TextureAsset.h"

namespace SE
{

TextureAsset::TextureAsset(RefPtr<Texture2D> renderer_texture, String texture_filepath)
    : Asset(AssetType::Texture)
    , m_renderer_texture(move(renderer_texture))
    , m_texture_filepath(move(texture_filepath))
{
}

} // namespace SE
