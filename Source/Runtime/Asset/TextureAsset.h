/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Asset/Asset.h>
#include <Core/Containers/RefPtr.h>
#include <Core/String/String.h>
#include <Renderer/Texture.h>

namespace SE
{

class TextureAsset : public Asset
{
public:
    NODISCARD ALWAYS_INLINE static AssetType get_static_type() { return AssetType::Texture; }
    virtual ~TextureAsset() override = default;

    SHOOTER_API TextureAsset(RefPtr<Texture2D> renderer_texture, String texture_filepath);

    NODISCARD ALWAYS_INLINE RefPtr<Texture2D> get_renderer_texture() const { return m_renderer_texture; }
    NODISCARD ALWAYS_INLINE const String& get_texture_filepath() const { return m_texture_filepath; }

private:
    RefPtr<Texture2D> m_renderer_texture;
    String m_texture_filepath;
};

} // namespace SE
