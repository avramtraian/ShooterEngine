// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/Texture.h>

#include <map>
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

struct RenderPassAttachment
{
public:
    std::shared_ptr<Texture2D> Texture { nullptr };
    AttachmentLoadOp LoadOp            { AttachmentLoadOp::Load };
    AttachmentStoreOp StoreOp          { AttachmentStoreOp::Store };

public:
    inline RenderPassAttachment& SetTexture (const std::shared_ptr<Texture2D>& texture) { Texture = texture; return *this; }
    inline RenderPassAttachment& SetLoadOp  (AttachmentLoadOp loadOp)                   { LoadOp = loadOp;   return *this; }
    inline RenderPassAttachment& SetStoreOp (AttachmentStoreOp storeOp)                 { StoreOp = storeOp; return *this; }
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

class CommandList
{
    SE_MAKE_RENDERER_RHI_INTERFACE(CommandList);

public:
    virtual void BeginRenderPass(const RenderPassInfo& renderPassInfo) = 0;
    virtual void EndRenderPass() = 0;
};

}
