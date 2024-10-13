/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/RefPtr.h>
#include <Core/Containers/Span.h>
#include <Renderer/Image.h>

namespace SE
{

struct Texture2DDescription
{
    u32 width { 0 };
    u32 height { 0 };
    ImageFormat format { ImageFormat::Unknown };
    ReadonlyByteSpan data;

    ImageFilteringMode min_filter { ImageFilteringMode::Linear };
    ImageFilteringMode mag_filter { ImageFilteringMode::Linear };
    ImageAddressMode address_mode_u { ImageAddressMode::ClampToEdge };
    ImageAddressMode address_mode_v { ImageAddressMode::ClampToEdge };
    ImageAddressMode address_mode_w { ImageAddressMode::ClampToEdge };
};

class Texture2D : public RefCounted
{
public:
    NODISCARD SHOOTER_API static RefPtr<Texture2D> create(const Texture2DDescription& info);
    
    Texture2D() = default;
    virtual ~Texture2D() override = default;

public:
    NODISCARD virtual u32 get_width() const = 0;
    NODISCARD virtual u32 get_height() const = 0;
    NODISCARD virtual ImageFormat get_format() const = 0;
};

} // namespace SE
