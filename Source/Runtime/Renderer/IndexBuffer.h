/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/RefPtr.h>
#include <Core/Containers/Span.h>

namespace SE
{

enum class IndexType : u8
{
    UInt16,
    UInt32,
};

struct IndexBufferInfo
{
    IndexType index_type;
    usize byte_count;
    ReadonlyByteSpan data;
};

class IndexBuffer : public RefCounted
{
public:
    IndexBuffer() = default;
    virtual ~IndexBuffer() override = default;

    static RefPtr<IndexBuffer> create(const IndexBufferInfo& info);

public:
};

} // namespace SE
