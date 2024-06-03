/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/RefPtr.h"
#include "Renderer/Image.h"

namespace SE
{

struct Texture2DInfo
{
    u32 width = 0;
    u32 height = 0;
    ImageFormat format = ImageFormat::Unknown;
    ImageFiltering min_filter = ImageFiltering::Linear;
    ImageFiltering mag_filter = ImageFiltering::Linear;
    ImageAddressMode address_mode_u = ImageAddressMode::ClampToEdge;
    ImageAddressMode address_mode_v = ImageAddressMode::ClampToEdge;
    ReadonlyByteSpan data;
};

class Texture2D : public RefCounted
{
public:
    Texture2D() = default;
    virtual ~Texture2D() override = default;

    SHOOTER_API static RefPtr<Texture2D> create(const Texture2DInfo& info);

public:
    virtual u32 get_width() const = 0;
    virtual u32 get_height() const = 0;
    virtual ImageFormat get_format() const = 0;
};

} // namespace SE
