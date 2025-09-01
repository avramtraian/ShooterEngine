// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Renderer/RHI/RHICore.h>

namespace SE
{

enum class GraphicsVertexInputAttributeType : uint8
{
    Unknown = 0,
    Float, Float2, Float3, Float4,
    Int, Int2, Int3, Int4,
    UInt, UInt2, UInt3, UInt4,
};

struct GraphicsVertexInputAttribute
{
    String Name;
    GraphicsVertexInputAttributeType Type { GraphicsVertexInputAttributeType::Unknown };
};

struct GraphicsVertexInputLayout
{
public:
    Vector<GraphicsVertexInputAttribute> Attributes;

public:
    inline GraphicsVertexInputLayout& AddAttribute(StringView name, GraphicsVertexInputAttributeType type)
    {
        GraphicsVertexInputAttribute& attribute = Attributes.Emplace();
        attribute.Name = name;
        attribute.Type = type;
        return *this;
    }
};

enum class GraphicsTopology : uint8
{
    Unknown = 0,
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    TriangleFan,
    LineListWithAdjaceny,
    LineStripWithAdjaceny,
    TriangleListWithdjaceny,
    TriangleStripWithdjaceny,
};

enum class GraphicsCullMode : uint8
{
    Disabled = 0,
    Back,
    Front,
};

enum class GraphicsFrontFace : uint8
{
    Clockwise = 0,
    CounterClockwise,
};

class GraphicsState
{
public:
    GraphicsVertexInputLayout VertexInputLayout;
    GraphicsTopology          Topology             { GraphicsTopology::Unknown };
    GraphicsCullMode          CullMode             { GraphicsCullMode::Disabled };
    GraphicsFrontFace         FrontFace            { GraphicsFrontFace::Clockwise };
    bool                      HasDepthAttachment   { false };
    bool                      HasStencilAttachment { false };
    bool                      EnableBlending       { true };

public:
    inline GraphicsState& SetVertexInputLayout         (GraphicsVertexInputLayout layout) { VertexInputLayout = std::move(layout);                            return *this; }
    inline GraphicsState& SetTopology                  (GraphicsTopology topology)        { Topology = topology;                                              return *this; }
    inline GraphicsState& SetCullMode                  (GraphicsCullMode cullMode)        { CullMode = cullMode;                                              return *this; }
    inline GraphicsState& SetFrontFace                 (GraphicsFrontFace frontFace)      { FrontFace = frontFace;                                            return *this; }
    inline GraphicsState& SetHasDepthAttachment        (bool value)                       { HasDepthAttachment = value;                                       return *this; }
    inline GraphicsState& SetHasStencilAttachment      (bool value)                       { HasStencilAttachment = value;                                     return *this; }
    inline GraphicsState& SetHasDepthStencilAttachment (bool hasDepth, bool hasStencil)   { HasDepthAttachment = hasDepth; HasStencilAttachment = hasStencil; return *this; }
    inline GraphicsState& SetEnableBlending            (bool value)                       { EnableBlending = value;                                           return *this; }
};

}
