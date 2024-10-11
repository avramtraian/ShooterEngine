/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Asset/AssetManager.h>
#include <Asset/TextureAsset.h>
#include <Engine/Scene/Components/SpriteRendererComponent.h>
#include <Engine/Scene/Components/TransformComponent.h>
#include <Renderer/Renderer.h>
#include <Renderer/SceneRenderer.h>

namespace SE
{

bool SceneRenderer::initialize(Scene& in_scene_context, u32 width, u32 height)
{
    SE_ASSERT(m_scene_context == nullptr);
    m_scene_context = &in_scene_context;

    SE_ASSERT(!m_renderer_2d.is_valid());
    m_renderer_2d = make_own<Renderer2D>();
    if (!m_renderer_2d->initialize(width, height))
    {
        SE_LOG_TAG_ERROR("Renderer"sv, "Failed to initialize the 2D renderer!"sv);
        return false;
    }

    m_debug_texture = g_asset_manager->get_asset_sync<TextureAsset>(AssetHandle(61034756314586))->get_renderer_texture();
    return true;
}

void SceneRenderer::shutdown()
{
    // Clear the scene context.
    m_scene_context = nullptr;

    m_renderer_2d->shutdown();
    m_renderer_2d.release();
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

            m_renderer_2d->submit_quad({ t.x, t.y }, { s.x, s.y }, tc.rotation(), src.sprite_color());
            return IterationDecision::Continue;
        }
    );

    m_renderer_2d->end_frame();
    Renderer::end_frame();
    return true;
}

void SceneRenderer::on_resize(u32 new_width, u32 new_height)
{
    // Resize the 2D renderer.
    m_renderer_2d->resize_target_framebuffer(new_width, new_height);
}

} // namespace SE
