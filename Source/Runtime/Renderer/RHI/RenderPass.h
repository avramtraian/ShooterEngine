// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Texture.h>

#include <memory>
#include <string>
#include <unordered_map>
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

struct RenderPassAttachment
{
public:
    TextureFormat     Format      { TextureFormat::Unknown };
    TextureLayout     FinalLayout { TextureLayout::ShaderReadOptimal };
    AttachmentLoadOp  LoadOp      { AttachmentLoadOp::Load };
    AttachmentStoreOp StoreOp     { AttachmentStoreOp::Store };

public:
    inline RenderPassAttachment& SetFormat      (TextureFormat format)      { Format = format;      return *this; }
    inline RenderPassAttachment& SetFinalLayout (TextureLayout layout)      { FinalLayout = layout; return *this; }
    inline RenderPassAttachment& SetLoadOp      (AttachmentLoadOp loadOp)   { LoadOp = loadOp;      return *this; }
    inline RenderPassAttachment& SetStoreOp     (AttachmentStoreOp storeOp) { StoreOp = storeOp;    return *this; }
};

struct RenderPassInfo
{
public:
    std::string DebugName;
    std::vector<RenderPassAttachment> ColorAttachments;
    bool HasDepthStencilAttachment { false };
    RenderPassAttachment DepthStencilAttachment;

public:
    inline RenderPassInfo& SetDebugName              (std::string_view debugName)      { DebugName = debugName;                                                 return *this; }
    inline RenderPassInfo& AddColorAttachment        (RenderPassAttachment attachment) { ColorAttachments.push_back(std::move(attachment));                     return *this; }
    inline RenderPassInfo& SetDepthStencilAttachment (RenderPassAttachment attachment) { DepthStencilAttachment = attachment; HasDepthStencilAttachment = true; return *this; }
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

struct RenderPassAttachmentTexture
{
public:
    RefPtr<Texture2D> Texture;
    RenderPassAttachmentClearValue ClearValue {};

public:
    inline RenderPassAttachmentTexture& SetTexture(const RefPtr<Texture2D>& texture) { Texture = texture; return *this; }

    inline RenderPassAttachmentTexture& SetClearValueFloat32(float r, float g, float b, float a)
    {
        ClearValue.Float32[0] = r;
        ClearValue.Float32[1] = g;
        ClearValue.Float32[2] = b;
        ClearValue.Float32[3] = a;
        return *this;
    }

    inline RenderPassAttachmentTexture& SetClearValueInt32(int32 r, int32 g, int32 b, int32 a)
    {
        ClearValue.Int32[0] = r;
        ClearValue.Int32[1] = g;
        ClearValue.Int32[2] = b;
        ClearValue.Int32[3] = a;
        return *this;
    }

    inline RenderPassAttachmentTexture& SetClearValueUInt32(uint32 r, uint32 g, uint32 b, uint32 a)
    {
        ClearValue.UInt32[0] = r;
        ClearValue.UInt32[1] = g;
        ClearValue.UInt32[2] = b;
        ClearValue.UInt32[3] = a;
        return *this;
    }
};

struct RenderPassBeginInfo
{
public:
    std::unordered_map<uint32, RenderPassAttachmentTexture> ColorAttachmentTextures;
    RenderPassAttachmentTexture DepthStencilAttachmentTexture;

public:
    inline RenderPassBeginInfo& AddColorAttachmentTexture(uint32 attachmentIndex, const RenderPassAttachmentTexture& attachmentTexture)
    {
        SE_ASSERT(!ColorAttachmentTextures.contains(attachmentIndex));
        ColorAttachmentTextures.insert({ attachmentIndex, attachmentTexture });
        return *this;
    }

    inline RenderPassBeginInfo& SetDepthStencilttachmentTexture(const RenderPassAttachmentTexture& attachmentTexture)
    {
        SE_ASSERT(!DepthStencilAttachmentTexture.Texture.IsValid());
        DepthStencilAttachmentTexture = attachmentTexture;
        return *this;
    }
};

class RenderPass : public RefCounted
{
    SE_MAKE_RENDERER_RHI_INTERFACE(RenderPass);
};

}
