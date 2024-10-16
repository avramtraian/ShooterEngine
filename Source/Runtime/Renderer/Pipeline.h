/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/RefPtr.h>
#include <Renderer/Shader.h>

namespace SE
{

enum class PipelineVertexAttributeType : u8
{
    // clang-format off
    Float1, Float2, Float3, Float4,
    Int1,   Int2,   Int3,   Int4,
    UInt1,  UInt2,  UInt3,  UInt4,
    // clang-format on
};

struct PipelineVertexAttribute
{
public:
    PipelineVertexAttribute() = default;
    PipelineVertexAttribute(PipelineVertexAttributeType in_type, StringView in_name)
        : type(in_type)
        , name(in_name)
    {}

public:
    PipelineVertexAttributeType type;
    String name;
};

enum class PipelinePrimitiveTopology : u8
{
    Unknown = 0,
    TriangleList,
};

enum class PipelineFillMode : u8
{
    Unknown = 0,
    Solid,
    Wireframe,
};

enum class PipelineCullMode : u8
{
    Unknown = 0,
    None,
    Front,
    Back,
};

enum class PipelineFrontFaceDirection : u8
{
    Unknown = 0,
    CounterClockwise,
    Clockwise,
};

struct PipelineDescription
{
    RefPtr<Shader> shader;
    Vector<PipelineVertexAttribute> vertex_attributes;

    PipelinePrimitiveTopology primitive_topology { PipelinePrimitiveTopology::TriangleList };
    PipelineFillMode fill_mode { PipelineFillMode::Solid };
    PipelineCullMode cull_mode { PipelineCullMode::None };
    PipelineFrontFaceDirection front_face_direction { PipelineFrontFaceDirection::CounterClockwise };
};

class Pipeline : public RefCounted
{
public:
    NODISCARD SHOOTER_API static RefPtr<Pipeline> create(const PipelineDescription& description);

    Pipeline() = default;
    virtual ~Pipeline() override = default;

public:
    NODISCARD virtual RefPtr<Shader> get_shader() const = 0;

    NODISCARD virtual PipelinePrimitiveTopology get_primitive_topology() const = 0;
    NODISCARD virtual PipelineFillMode get_fill_mode() const = 0;
    NODISCARD virtual PipelineCullMode get_cull_mode() const = 0;
    NODISCARD virtual PipelineFrontFaceDirection get_front_face_direction() const = 0;
};

} // namespace SE
