// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/RHIObject.h>

namespace SE
{

class ShaderResource : public RHIObject
{
public:
    ShaderResource() = default;
    virtual ~ShaderResource() override = default;
};

enum PipelineStageBitsEnum : uint64
{
    PIPELINE_STAGE_NONE_BIT                    = 0,
    PIPELINE_STAGE_TOP_OF_PIPE_BIT             = BIT(0),
    PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = BIT(1),
    PIPELINE_STAGE_TRANSFER_BIT                = BIT(2),
    PIPELINE_STAGE_VERTEX_SHADER_BIT           = BIT(3),
    PIPELINE_STAGE_FRAGMENT_SHADER_BIT         = BIT(4),
};
using PipelineStageBits = uint64;

enum class TextureLayout : uint16
{
    Undefined = 0,
    PresentSource,
    ColorAttachmentOptimal,
    DepthAttachmentOptimal,
    StencilAttachmentOptimal,
    DepthStencilAttachmentOptimal,
    ShaderReadOnlyOptimal,
    TransferDstOptimal,
    TransferSrcOptimal,
};

enum AccessFlagsBitsEnum : uint64
{
    ACCESS_FLAG_NONE_BIT                           = 0,
    ACCESS_FLAG_VERTEX_ATTRIBUTE_READ_BIT          = BIT(0),
    ACCESS_FLAG_UNIFORM_READ_BIT                   = BIT(1),
    ACCESS_FLAG_INPUT_ATTACHMENT_READ_BIT          = BIT(2),
    ACCESS_FLAG_SHADER_READ_BIT                    = BIT(3),
    ACCESS_FLAG_SHADER_WRITE_BIT                   = BIT(4),
    ACCESS_FLAG_COLOR_ATTACHMENT_READ_BIT          = BIT(5),
    ACCESS_FLAG_COLOR_ATTACHMENT_WRITE_BIT         = BIT(6),
    ACCESS_FLAG_DEPTH_STENCIL_ATTACHMENT_READ_BIT  = BIT(7),
    ACCESS_FLAG_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = BIT(8),
    ACCESS_FLAG_TRANSFER_READ_BIT                  = BIT(9),
    ACCESS_FLAG_TRANSFER_WRITE_BIT                 = BIT(10),
};
using AccessFlagsBits = uint64;

}
