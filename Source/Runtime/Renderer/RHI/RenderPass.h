// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>

#include <memory>
#include <string>
#include <vector>

namespace SE
{

enum class AttachmentLoadOp : uint8
{
    Load,
    Clear,
    DontCare,
};

enum class AttachmentStoreOp : uint8
{
    Store,
    DontCare,
};

union RenderPassAttachmentClearValue
{
    /* Color attachment clear values. */
    float Float32[4];
    int32 Int32[4];
    uint32 UInt32[4];

    /* Depth-stencil attachment clear values. */
    struct
    {
        float Depth;
        uint32 Stencil;
    };
};

struct RenderPassAttachment
{
public:
    std::shared_ptr<Texture2D>     Texture    { nullptr };
    AttachmentLoadOp               LoadOp     { AttachmentLoadOp::Load };
    AttachmentStoreOp              StoreOp    { AttachmentStoreOp::Store };
    RenderPassAttachmentClearValue ClearValue {};

public:
    inline RenderPassAttachment& SetTexture    (const std::shared_ptr<Texture2D>& texture) { Texture = texture; return *this; }
    inline RenderPassAttachment& SetLoadOp     (AttachmentLoadOp loadOp)                   { LoadOp = loadOp;   return *this; }
    inline RenderPassAttachment& SetStoreOp    (AttachmentStoreOp storeOp)                 { StoreOp = storeOp; return *this; }

    inline RenderPassAttachment& SetClearValueFloat32 (float r, float g, float b, float a)
    {
        ClearValue.Float32[0] = r;
        ClearValue.Float32[1] = g;
        ClearValue.Float32[2] = b;
        ClearValue.Float32[3] = a;
        return *this;
    }

    inline RenderPassAttachment& SetClearValueInt32(int32 r, int32 g, int32 b, int32 a)
    {
        ClearValue.Int32[0] = r;
        ClearValue.Int32[1] = g;
        ClearValue.Int32[2] = b;
        ClearValue.Int32[3] = a;
        return *this;
    }

    inline RenderPassAttachment& SetClearValueUInt32(uint32 r, uint32 g, uint32 b, uint32 a)
    {
        ClearValue.UInt32[0] = r;
        ClearValue.UInt32[1] = g;
        ClearValue.UInt32[2] = b;
        ClearValue.UInt32[3] = a;
        return *this;
    }
};

struct RenderPassInfo
{
public:
    std::string DebugName;
    std::vector<RenderPassAttachment> ColorAttachments;
    RenderPassAttachment DepthStencilAttachment;

public:
    inline RenderPassInfo& SetDebugName              (std::string_view debugName)      { DebugName = debugName;                             return *this; }
    inline RenderPassInfo& AddColorAttachment        (RenderPassAttachment attachment) { ColorAttachments.push_back(std::move(attachment)); return *this; }
    inline RenderPassInfo& SetDepthStencilAttachment (RenderPassAttachment attachment) { DepthStencilAttachment = attachment;               return *this; }
};

class RenderPass
{
    SE_MAKE_RENDERER_RHI_INTERFACE(RenderPass);
};

}
