// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Shader.h>

#include <memory>

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
    std::string Name;
    GraphicsVertexInputAttributeType Type { GraphicsVertexInputAttributeType::Unknown };
};

struct GraphicsVertexInputLayout
{
public:
    std::vector<GraphicsVertexInputAttribute> Attributes;

public:
    inline GraphicsVertexInputLayout& AddAttribute(std::string_view name, GraphicsVertexInputAttributeType type)
    {
        GraphicsVertexInputAttribute& attribute = Attributes.emplace_back();
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

enum GraphicStateStageEnum : uint32
{
    GRAPHICS_STATE_STAGE_BIT_NONE     = 0,
    GRAPHICS_STATE_STAGE_BIT_VERTEX   = BIT(0),
    GRAPHICS_STATE_STAGE_BIT_FRAGMENT = BIT(1),
};
using GraphicsStateStageBits = uint32;

enum class GraphicsStateResourceType : uint16
{
    Unknown = 0,
    Texture2D,
};

struct GraphicsResourceBinding
{
public:
    int32                     BindingIndex { -1 };
    GraphicsStateStageBits    Stages       { GRAPHICS_STATE_STAGE_BIT_NONE };
    GraphicsStateResourceType ResourceType { GraphicsStateResourceType::Unknown };
    uint32                    ArrayCount   { 1 };

public:
    inline GraphicsResourceBinding& SetBindingIndex (uint32 bindingIndex)                    { BindingIndex = bindingIndex; return *this; }
    inline GraphicsResourceBinding& SetStages       (GraphicsStateStageBits stages)          { Stages = stages;             return *this; }
    inline GraphicsResourceBinding& SetResourceType (GraphicsStateResourceType resourceType) { ResourceType = resourceType; return *this; }
    inline GraphicsResourceBinding& SetArrayCount   (uint32 arrayCount)                      { ArrayCount = arrayCount;     return *this; }
};

struct GraphicsResourceSet
{
public:
    int32 SetIndex { -1 };
    std::vector<GraphicsResourceBinding> Bindings;

public:
    inline GraphicsResourceSet& SetSetIndex (uint32 setIndex)                 { SetIndex = setIndex;                    return *this; }
    inline GraphicsResourceSet& AddBinding  (GraphicsResourceBinding binding) { Bindings.push_back(std::move(binding)); return *this; }
};

class GraphicsState
{
public:
    std::shared_ptr<Shader>          Shader;
    GraphicsVertexInputLayout        VertexInputLayout;
    GraphicsTopology                 Topology             { GraphicsTopology::Unknown };
    GraphicsCullMode                 CullMode             { GraphicsCullMode::Disabled };
    GraphicsFrontFace                FrontFace            { GraphicsFrontFace::Clockwise };
    bool                             HasDepthAttachment   { false };
    bool                             HasStencilAttachment { false };
    bool                             EnableBlending       { true };
    std::vector<GraphicsResourceSet> ResourceSets;

public:
    inline GraphicsState& SetShader                    (std::shared_ptr<class Shader> shader) { Shader = std::move(shader);                                       return *this; }
    inline GraphicsState& SetVertexInputLayout         (GraphicsVertexInputLayout layout)     { VertexInputLayout = std::move(layout);                            return *this; }
    inline GraphicsState& SetTopology                  (GraphicsTopology topology)            { Topology = topology;                                              return *this; }
    inline GraphicsState& SetCullMode                  (GraphicsCullMode cullMode)            { CullMode = cullMode;                                              return *this; }
    inline GraphicsState& SetFrontFace                 (GraphicsFrontFace frontFace)          { FrontFace = frontFace;                                            return *this; }
    inline GraphicsState& SetHasDepthAttachment        (bool value)                           { HasDepthAttachment = value;                                       return *this; }
    inline GraphicsState& SetHasStencilAttachment      (bool value)                           { HasStencilAttachment = value;                                     return *this; }
    inline GraphicsState& SetHasDepthStencilAttachment (bool hasDepth, bool hasStencil)       { HasDepthAttachment = hasDepth; HasStencilAttachment = hasStencil; return *this; }
    inline GraphicsState& SetEnableBlending            (bool value)                           { EnableBlending = value;                                           return *this; }
    inline GraphicsState& AddResourceSet               (GraphicsResourceSet set)              { ResourceSets.push_back(std::move(set));                           return *this; }
};

}
