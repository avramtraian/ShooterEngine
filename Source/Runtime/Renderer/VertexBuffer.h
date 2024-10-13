/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/RefPtr.h>
#include <Core/Containers/Span.h>

namespace SE
{

enum class VertexBufferUpdateFrequency
{
    Never,
    Normal,
    High,
};

struct VertexBufferDescription
{
    u32 byte_count = 0;
    VertexBufferUpdateFrequency update_frequency = VertexBufferUpdateFrequency::Normal;
    ReadonlyByteSpan data;
};

class VertexBuffer : public RefCounted
{
public:
    NODISCARD static RefPtr<VertexBuffer> create(const VertexBufferDescription& description);

    VertexBuffer() = default;
    virtual ~VertexBuffer() override = default;

public:
    virtual void update_data(ReadonlyByteSpan data) = 0;
};

} // namespace SE
