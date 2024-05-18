/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Asset/Asset.h"

namespace SE
{

class TextureAsset : public Asset
{
public:
    NODISCARD ALWAYS_INLINE static AssetType get_static_type() { return AssetType::Texture; }
    virtual ~TextureAsset() override = default;

    SHOOTER_API TextureAsset(String texture_filepath);

    NODISCARD ALWAYS_INLINE const String& get_texture_filepath() const { return m_texture_filepath; }

private:
    String m_texture_filepath;
};

} // namespace SE
