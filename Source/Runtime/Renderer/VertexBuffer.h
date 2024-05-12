/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/RefPtr.h"

namespace SE
{

struct VertexBufferInfo
{
    usize            byte_count;
    ReadonlyByteSpan data;
};

class VertexBuffer : public RefCounted
{
public:
    VertexBuffer() = default;
    virtual ~VertexBuffer() override = default;
    
    static RefPtr<VertexBuffer> create(const VertexBufferInfo& info);

public:
};

} // namespace SE