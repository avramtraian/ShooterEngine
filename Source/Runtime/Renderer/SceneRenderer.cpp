/*
 * Copyright (c) 2024-2025 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Runtime/Renderer/RHI/CommandList.h>
#include <Runtime/Renderer/RHI/RenderPass.h>
#include <Runtime/Renderer/RHI/RenderingDriver.h>
#include <Runtime/Renderer/RHI/RenderingSurface.h>
#include <Runtime/Renderer/SceneRenderer.h>
#include <Runtime/Scene/Components/SpriteRendererComponent.h>
#include <Runtime/Scene/Components/TransformComponent.h>

namespace SE
{

RefPtr<SceneRenderer> SceneRenderer::Create(const SceneRendererInfo& info)
{
    return CreateStrongRefWithFunction<SceneRenderer>([&](void* memoryBlock) { new (memoryBlock) SceneRenderer(info); });
}

SceneRenderer::SceneRenderer(const SceneRendererInfo& info)
    : m_SceneContext(info.SceneContext)
{
    m_Renderer2D = Renderer2D::Create(Renderer2DInfo()
                                          .SetMaxFramesInFlight(info.MaxFramesInFlight)
                                          .SetRenderToSwapchainTarget(info.RenderToSwapchainTarget)
                                          .SetRenderTargetFormat(info.RenderTargetFormat)
                                          .SetTextureSlotsPerDrawCall(16)
                                          .SetMaxQuadsPerDrawCall(4096));
}

void SceneRenderer::Render(SceneRendererRenderInfo renderInfo)
{
    m_Renderer2D->BeginFrame(
        Renderer2DBeginFrameInfo().SetFrameIndex(renderInfo.FrameIndex).SetCommandList(renderInfo.CommandList).SetRenderTarget(renderInfo.RenderTarget));

    const Vector<EntityID> entities = m_SceneContext->QueryAllEntities();
    for (EntityID entityID : entities)
    {
        Entity entity = m_SceneContext->GetEntityFromID(entityID);
        if (entity.HasComponent<TransformComponent>() && entity.HasComponent<SpriteRendererComponent>())
        {
            const TransformComponent&      transformComponent      = entity.GetComponent<TransformComponent>();
            const SpriteRendererComponent& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();

            Renderer2DQuad quad = {};
            quad.Translation    = transformComponent.Translation;
            quad.Rotation       = transformComponent.Rotation;
            quad.Scale          = transformComponent.Scale.XY;
            quad.MinUV          = Vector2(0.0F, 0.0F);
            quad.MaxUV          = Vector2(1.0F, 1.0F);
            quad.Color          = spriteRendererComponent.TintColor;
            m_Renderer2D->SubmitQuad(quad);
        }
    }

    m_Renderer2D->EndFrame();
}

} // namespace SE
