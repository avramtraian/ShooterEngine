/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/RefPtr.h"

namespace SE
{

enum class VertexBufferUpdateFrequency
{
    Never,
    Normal,
    High,
};

struct VertexBufferInfo
{
    u32 byte_count = 0;
    VertexBufferUpdateFrequency update_frequency = VertexBufferUpdateFrequency::Normal;
    ReadonlyByteSpan data;
};

class VertexBuffer : public RefCounted
{
public:
    VertexBuffer() = default;
    virtual ~VertexBuffer() override = default;
    
    static RefPtr<VertexBuffer> create(const VertexBufferInfo& info);

public:
    virtual void update_data(ReadonlyByteSpan data) = 0;
};

} // namespace SE
