/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Runtime/Core/Containers/RefPtr.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Math/Vector.h>
#include <Runtime/Renderer/RHI/Buffer.h>
#include <Runtime/Renderer/RHI/Texture.h>

namespace SE
{

struct Renderer2DInfo
{
public:
    uint32        MaxFramesInFlight       = 0;
    bool          RenderToSwapchainTarget = false;
    TextureFormat RenderTargetFormat      = TextureFormat::Unknown;

    // NOTE: Maybe we should query this parameter directly from the RHI layer in the Renderer2D implementation
    //       instead of expecting it from the initialization info. Are there any reasons this shouldn't be the
    //       maximum that the graphics drivers allows?
    uint32 TextureSlotsPerDrawCall = 0;

    uint32 MaxQuadsPerDrawCall = 0;

public:
    // clang-format off
    inline Renderer2DInfo& SetMaxFramesInFlight       (uint32 maxFramesInFlight)       { MaxFramesInFlight = maxFramesInFlight;             return *this; }
    inline Renderer2DInfo& SetRenderToSwapchainTarget (bool value)                     { RenderToSwapchainTarget = value;                   return *this; }
    inline Renderer2DInfo& SetRenderTargetFormat      (TextureFormat format)           { RenderTargetFormat = format;                       return *this; }
    inline Renderer2DInfo& SetTextureSlotsPerDrawCall (uint32 textureSlotsPerDrawCall) { TextureSlotsPerDrawCall = textureSlotsPerDrawCall; return *this; }
    inline Renderer2DInfo& SetMaxQuadsPerDrawCall     (uint32 maxQuadsPerDrawCall)     { MaxQuadsPerDrawCall = maxQuadsPerDrawCall;         return *this; }
    // clang-format on
};

struct Renderer2DBeginFrameInfo
{
public:
    uint32                    FrameIndex;
    StrongRefPtr<CommandList> CommandList;
    StrongRefPtr<Texture2D>   RenderTarget;

public:
    // clang-format off
    inline Renderer2DBeginFrameInfo& SetFrameIndex   (uint32 frameIndex)                           { FrameIndex = frameIndex;           return *this; }
    inline Renderer2DBeginFrameInfo& SetCommandList  (StrongRefPtr<class CommandList> commandList) { CommandList = Move(commandList);   return *this; }
    inline Renderer2DBeginFrameInfo& SetRenderTarget (StrongRefPtr<Texture2D> renderTarget)        { RenderTarget = Move(renderTarget); return *this; }
    // clang-format on
};

struct Renderer2DQuad
{
    Vector3           Translation;
    Vector3           Rotation;
    Vector2           Scale;
    Vector2           MinUV;
    Vector2           MaxUV;
    Vector4           Color;
    RefPtr<Texture2D> Texture;
};

struct Renderer2DQuadVertex
{
    Vector3 Position;
    Vector2 TexCoord;
    Vector4 Color;
    uint32  TextureSlotIndex;
};

class Renderer2D : public RefCounted
{
public:
    static constexpr usize VERTICES_PER_QUAD = 4;
    static constexpr usize INDICES_PER_QUAD  = 6;

public:
    NODISCARD RUNTIME_API static RefPtr<Renderer2D> Create(const Renderer2DInfo& info);

public:
    virtual ~Renderer2D() override = default;

    RUNTIME_API void BeginFrame(Renderer2DBeginFrameInfo beginFrameInfo);
    RUNTIME_API void EndFrame();

    RUNTIME_API void SubmitQuad(const Renderer2DQuad& quad);

    NODISCARD RUNTIME_API uint32 GetQuadCount() const { return static_cast<uint32>(m_QuadVertices.Count() / 4); }
    NODISCARD RUNTIME_API uint32 GetMaxQuadsPerDrawCall() const { return m_Info.MaxQuadsPerDrawCall; }

private:
    Renderer2D(const Renderer2DInfo& info);

    NODISCARD Optional<uint32> FindTextureSlotIndex(const RefPtr<Texture2D>& texture);

private:
    Renderer2DInfo                     m_Info {};
    Optional<Renderer2DBeginFrameInfo> m_BeginFrameInfo {};
    Vector<StrongRefPtr<Texture2D>>    m_TextureSlots;

    StrongRefPtr<RenderPass>           m_QuadRenderPass;
    Vector<StrongRefPtr<VertexBuffer>> m_QuadVertexBuffers;
    StrongRefPtr<IndexBuffer>          m_QuadIndexBuffer;
    Vector<Renderer2DQuadVertex>       m_QuadVertices;
};

} // namespace SE
