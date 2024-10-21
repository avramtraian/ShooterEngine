/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/StringBuilder.h>
#include <Core/FileSystem/FileSystem.h>
#include <Core/Log.h>
#include <Core/Platform/Timer.h>
#include <EditorAsset/EditorAssetManager.h>
#include <EditorEngine.h>
#include <Engine/Application/Events/WindowEvents.h>
#include <Renderer/Renderer.h>

namespace SE
{

EditorEngine* g_editor_engine = nullptr;

bool EditorEngine::initialize()
{
    if (!Engine::initialize())
        return false;
    g_editor_engine = this;

    if (!m_editor_context.pre_initialize())
    {
        SE_LOG_ERROR("Failed to pre initialize the editor context!");
        return false;
    }

    AssetManager::instantiate<EditorAssetManager>();
    if (!g_asset_manager->initialize())
    {
        SE_LOG_ERROR("Failed to initialize the asset manager!");
        return false;
    }

    if (!Renderer::initialize())
    {
        SE_LOG_ERROR("Failed to initialize the renderer!");
        return false;
    }

    if (!m_editor_context.initialize())
    {
        SE_LOG_ERROR("Failed to initialize the editor context!");
        return false;
    }

    if (!m_editor_context.post_initialize())
    {
        SE_LOG_ERROR("Failed to post initialize the editor context!");
        return false;
    }

    return true;
}

void EditorEngine::shutdown()
{
    m_editor_context.shutdown();

    Renderer::shutdown();
    g_asset_manager->shutdown();

    g_editor_engine = nullptr;
    Engine::shutdown();
}

void EditorEngine::update()
{
    Timer current_frame_timer;

    Engine::update();
    m_editor_context.on_update(m_last_frame_delta_time);

    current_frame_timer.stop();
    m_last_frame_delta_time = current_frame_timer.elapsed_seconds();
}

String EditorEngine::get_engine_root_directory() const
{
    const String engine_root_directory = m_editor_context.get_engine_root_directory();
    return engine_root_directory;
}

void EditorEngine::on_event(const Event& in_event)
{
    switch (in_event.get_type())
    {
        case EventType::WindowResized:
        {
            const WindowResizedEvent& e = static_cast<const WindowResizedEvent&>(in_event);
            const u32 new_width = e.get_client_width();
            const u32 new_height = e.get_client_height();

            if (Renderer::is_initialized())
            {
                // Propagate the event to the renderer.
                Renderer::on_resize(new_width, new_height);
            }

            break;
        }
    }
}

} // namespace SE
