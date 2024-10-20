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

enum class UniformBufferUsage : u8
{
    Unknown = 0,
    Immutable,
    Default,
    Dynamic,
};

struct UniformBufferDescription
{
    u32 byte_count { 0 };
    UniformBufferUsage usage { UniformBufferUsage::Default };
    ReadonlyByteSpan data;
};

class UniformBuffer : public RefCounted
{
public:
    NODISCARD SHOOTER_API static RefPtr<UniformBuffer> create(const UniformBufferDescription& description);

    UniformBuffer() = default;
    virtual ~UniformBuffer() override = default;

public:
    virtual void upload_data(ReadonlyByteSpan data) = 0;
};

} // namespace SE
