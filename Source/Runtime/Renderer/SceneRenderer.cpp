/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Asset/AssetManager.h>
#include <Asset/TextureAsset.h>
#include <Core/Log.h>
#include <Engine/Scene/Components/SpriteRendererComponent.h>
#include <Engine/Scene/Components/TransformComponent.h>
#include <Renderer/Renderer.h>
#include <Renderer/SceneRenderer.h>

namespace SE
{

bool SceneRenderer::initialize(Scene& in_scene_context, RefPtr<Framebuffer> target_framebuffer)
{
    SE_ASSERT(m_scene_context == nullptr);
    m_scene_context = &in_scene_context;
    m_target_framebuffer = move(target_framebuffer);

    m_renderer_2d = create_own<Renderer2D>();
    if (!m_renderer_2d->initialize(m_target_framebuffer))
    {
        SE_LOG_TAG_ERROR("Renderer", "Failed to initialize the 2D renderer!");
        return false;
    }

    return true;
}

void SceneRenderer::shutdown()
{
    // Clear the scene context.
    m_scene_context = nullptr;

    m_renderer_2d->shutdown();
    m_renderer_2d.release();

    m_target_framebuffer.release();
}

void SceneRenderer::on_resize(u32 new_width, u32 new_height)
{
    // Resize the 2D renderer.
    m_renderer_2d->on_resize(new_width, new_height);
}

bool SceneRenderer::render()
{
    Renderer::begin_frame();
    m_renderer_2d->begin_frame();

    m_scene_context->for_each_entity(
        [&](const Entity* entity, UUID)
        {
            if (!entity->has_component<TransformComponent>() || !entity->has_component<SpriteRendererComponent>())
                return IterationDecision::Continue;

            const TransformComponent& tc = entity->get_component<TransformComponent>();
            const SpriteRendererComponent& src = entity->get_component<SpriteRendererComponent>();

            const Vector3 t = tc.translation();
            const Vector3 s = tc.scale();

            m_renderer_2d->submit_quad({ t.x, t.y }, { s.x, s.y }, src.sprite_color());
            return IterationDecision::Continue;
        }
    );

    m_renderer_2d->end_frame();
    Renderer::end_frame();
    return true;
}

} // namespace SE
