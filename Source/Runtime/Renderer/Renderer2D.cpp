/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Runtime/Core/Log.h>
#include <Runtime/Core/Math/Matrix.h>
#include <Runtime/Core/Math/MatrixTransformations.h>
#include <Runtime/Renderer/RHI/RenderingDriver.h>
#include <Runtime/Renderer/Renderer2D.h>

namespace SE
{

RefPtr<Renderer2D> Renderer2D::Create(const Renderer2DInfo& info)
{
    return CreateStrongRefWithFunction<Renderer2D>([&](void* memoryBlock) { new (memoryBlock) Renderer2D(info); });
}

Renderer2D::Renderer2D(const Renderer2DInfo& info)
    : m_Info(info)
{
    // clang-format off
    m_QuadRenderPass = g_RenderingDriver->CreateRenderPass(RenderPassInfo()
        .AddColorAttachment(RenderPassAttachment()
            .SetFormat(m_Info.RenderTargetFormat)
            .SetFinalLayout(m_Info.RenderToSwapchainTarget ? TextureLayout::PresentSource : TextureLayout::ColorAttachmentOptimal)
            .SetLoadOp(AttachmentLoadOp::Clear)
            .SetStoreOp(AttachmentStoreOp::Store)));
    // clang-format on

    m_QuadVertexBuffers.EnsureCapacity(m_Info.MaxFramesInFlight);
    for (uint32 quadVertexBufferIndex = 0; quadVertexBufferIndex < m_Info.MaxFramesInFlight; ++quadVertexBufferIndex)
    {
        const usize                vertexBufferSize = m_Info.MaxQuadsPerDrawCall * VERTICES_PER_QUAD * sizeof(Renderer2DQuadVertex);
        StrongRefPtr<VertexBuffer> vertexBuffer     = g_RenderingDriver->CreateVertexBuffer(VertexBufferInfo().SetBufferSize(vertexBufferSize));
        m_QuadVertexBuffers.Add(Move(vertexBuffer));
    }

    Vector<uint32> quadIndices;
    quadIndices.SetCountDefaulted(m_Info.MaxQuadsPerDrawCall * INDICES_PER_QUAD);
    for (uint32 quadIndex = 0; quadIndex < m_Info.MaxQuadsPerDrawCall; ++quadIndex)
    {
        quadIndices[(6 * quadIndex) + 0] = (4 * quadIndex) + 0;
        quadIndices[(6 * quadIndex) + 1] = (4 * quadIndex) + 1;
        quadIndices[(6 * quadIndex) + 2] = (4 * quadIndex) + 2;
        quadIndices[(6 * quadIndex) + 3] = (4 * quadIndex) + 2;
        quadIndices[(6 * quadIndex) + 4] = (4 * quadIndex) + 3;
        quadIndices[(6 * quadIndex) + 5] = (4 * quadIndex) + 0;
    }

    m_QuadIndexBuffer = g_RenderingDriver->CreateIndexBuffer(IndexBufferInfo()
                                                                 .SetDataType(IndexBufferDataType::UInt32)
                                                                 .SetIndexCount(static_cast<uint32>(quadIndices.Count()))
                                                                 .SetInitialIndices(quadIndices.Elements(), static_cast<uint32>(quadIndices.Count())));

    m_QuadVertices.EnsureCapacity(info.MaxQuadsPerDrawCall * VERTICES_PER_QUAD);
}

void Renderer2D::BeginFrame(Renderer2DBeginFrameInfo beginFrameInfo)
{
    SE_ASSERT(!m_BeginFrameInfo.HasValue());
    m_BeginFrameInfo = Move(beginFrameInfo);

    SE_ASSERT(m_BeginFrameInfo->RenderTarget->IsSwapchainTarget() == m_Info.RenderToSwapchainTarget);
    SE_ASSERT(m_BeginFrameInfo->RenderTarget->GetFormat() == m_Info.RenderTargetFormat);

    m_QuadVertices.Clear();
    m_TextureSlots.Clear();
    m_TextureSlots.Add(g_RenderingDriver->GetWhiteTexture());

    // clang-format off
    m_BeginFrameInfo->CommandList->BeginRenderPass(m_QuadRenderPass, RenderPassBeginInfo()
        .AddColorAttachmentTexture(0, RenderPassAttachmentTexture()
            .SetTexture(m_BeginFrameInfo->RenderTarget)
            .SetClearValueFloat32(0.5F, 0.5F, 0.5F, 1.0F)));
    // clang-format on
}

void Renderer2D::EndFrame()
{
    m_BeginFrameInfo->CommandList->EndRenderPass();

    SE_ASSERT(m_BeginFrameInfo.HasValue());
    m_BeginFrameInfo.Clear();
}

void Renderer2D::SubmitQuad(const Renderer2DQuad& quad)
{
    Optional<uint32> textureSlotIndex = FindTextureSlotIndex(quad.Texture);
    if (GetQuadCount() >= GetMaxQuadsPerDrawCall() || !textureSlotIndex.HasValue())
    {
        // TODO: Flush the quad buffer by performing a draw call instead of ignoring the submission call.
        return;
    }

    // NOTE: Sanity checks to ensure that no buffer overflow occurs.
    SE_ASSERT(m_QuadVertices.Count() + 4 <= m_QuadVertices.Capacity());

    // NOTE: These vertices are declared in CCW order.
    Renderer2DQuadVertex& vertex0 = m_QuadVertices.Emplace();
    Renderer2DQuadVertex& vertex1 = m_QuadVertices.Emplace();
    Renderer2DQuadVertex& vertex2 = m_QuadVertices.Emplace();
    Renderer2DQuadVertex& vertex3 = m_QuadVertices.Emplace();

    const Matrix4 transform = Matrix4::Scale(quad.Scale.X, quad.Scale.Y, 1.0F) * Matrix4::Rotate(quad.Rotation) * Matrix4::Translate(quad.Translation);

    vertex0.Position = (Vector4(-0.5F, -0.5F, 0.0F, 1.0F) * transform).XYZ;
    vertex1.Position = (Vector4(-0.5F, +0.5F, 0.0F, 1.0F) * transform).XYZ;
    vertex2.Position = (Vector4(+0.5F, +0.5F, 0.0F, 1.0F) * transform).XYZ;
    vertex3.Position = (Vector4(+0.5F, -0.5F, 0.0F, 1.0F) * transform).XYZ;

    vertex0.Color = quad.Color;
    vertex1.Color = quad.Color;
    vertex2.Color = quad.Color;
    vertex3.Color = quad.Color;

    vertex0.TexCoord = Vector2(quad.MinUV.X, quad.MinUV.Y);
    vertex1.TexCoord = Vector2(quad.MinUV.X, quad.MaxUV.Y);
    vertex2.TexCoord = Vector2(quad.MaxUV.X, quad.MaxUV.Y);
    vertex3.TexCoord = Vector2(quad.MaxUV.X, quad.MinUV.Y);

    if (!textureSlotIndex.HasValue())
    {
        // NOTE: We checked at the beginning of this function if there are any texture slots available, and if not we flushed
        //       the buffer - it is not possible to not have any available texture slots at this point.
        textureSlotIndex = FindTextureSlotIndex(quad.Texture);
        SE_ASSERT(textureSlotIndex.HasValue());
    }

    vertex0.TextureSlotIndex = textureSlotIndex.Value();
    vertex1.TextureSlotIndex = textureSlotIndex.Value();
    vertex2.TextureSlotIndex = textureSlotIndex.Value();
    vertex3.TextureSlotIndex = textureSlotIndex.Value();
}

Optional<uint32> Renderer2D::FindTextureSlotIndex(const RefPtr<Texture2D>& texture)
{
    // The first slot should always be occupied by the 1x1 white texture.
    SE_ASSERT(m_TextureSlots.HasElements() && m_TextureSlots.First().IsValid());
    if (!texture.IsValid())
        return 0;

    for (uint32 textureSlotIndex = 1; textureSlotIndex < m_TextureSlots.Count(); ++textureSlotIndex)
    {
        if (m_TextureSlots[textureSlotIndex].Get() == texture.Get())
            return textureSlotIndex;
    }

    if (m_TextureSlots.Count() >= m_Info.TextureSlotsPerDrawCall)
        return {};

    const uint32 textureSlotIndex = static_cast<uint32>(m_TextureSlots.Count());
    m_TextureSlots.Add(texture);
    return textureSlotIndex;
}

} // namespace SE
