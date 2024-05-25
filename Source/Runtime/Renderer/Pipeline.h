/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Renderer/Shader.h"

namespace SE
{

struct VertexAttribute
{
public:
    enum Type : u8
    {
        Float1, Float2, Float3, Float4,
        Int1,   Int2,   Int3,   Int4,
        UInt1,  UInt2,  UInt3,  UInt4,
        Mat2,   Mat3,   Mat4,
    };

public:
    VertexAttribute() = default;
    VertexAttribute(Type in_type, StringView in_name)
        : type(in_type)
        , name(in_name)
    {}

public:
    Type type;
    String name;
};

struct PipelineLayout
{
    PipelineLayout() = default;
    PipelineLayout(std::initializer_list<VertexAttribute> in_attributes)
        : attributes(in_attributes)
    {}

    Vector<VertexAttribute> attributes;
};

enum class PrimitiveTopology : u8
{
    TriangleList,
};

enum class FrontFace : u8
{
    CounterClockwise,
    Clockwise,
};

struct PipelineInfo
{
    PipelineLayout layout;
    RefPtr<Shader> shader;
    PrimitiveTopology primitive_topology = PrimitiveTopology::TriangleList;
    FrontFace front_face = FrontFace::Clockwise;
    bool enable_culling = false;
};

class Pipeline : public RefCounted
{
public:
    Pipeline() = default;
    virtual ~Pipeline() override = default;

    static RefPtr<Pipeline> create(const PipelineInfo& info);

public:
};

} // namespace SE
