/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/StringBuilder.h>
#include <Core/Log.h>
#include <EditorAsset/EditorAssetManager.h>
#include <EditorEngine.h>
#include <Engine/Application/WindowEvent.h>
#include <Renderer/Renderer.h>

namespace SE
{

EditorEngine* g_editor_engine = nullptr;

bool EditorEngine::initialize()
{
    if (!Engine::initialize())
        return false;
    g_editor_engine = this;

    // The current working is the engine root, so no redirection is necessary.
    m_engine_root_directory = ""sv;
    // Currently, there is no way to select/set the project you want to open in the
    // editor, thus the engine always picks the default example project.
    m_project_name = "ExampleProject"sv;
    m_project_root_directory = StringBuilder::path_join({ m_engine_root_directory.view(), "Content/ExampleProject"sv });

    AssetManager::instantiate<EditorAssetManager>();
    if (!g_asset_manager->initialize())
    {
        SE_LOG_ERROR("Failed to initialize the asset manager!"sv);
        return false;
    }

    if (!Renderer::initialize())
    {
        SE_LOG_ERROR("Failed to initialize the renderer!"sv);
        return false;
    }

    SE_LOG_INFO("Creating the primary window..."sv);
    if (!g_editor_engine->create_window())
    {
        SE_LOG_ERROR("Failed to create the primary window!"sv);
        return false;
    }

    Renderer::create_context_for_window(m_window_stack.first().get());
    RenderingContext* primary_window_rendering_context = Renderer::get_rendering_context_for_window(m_window_stack.first().get());
    Renderer::set_active_context(primary_window_rendering_context);

    // Creat the scene object.
    m_scene = Scene::create();

    Window* window = m_window_stack.first().get();
    m_scene_renderer = make_own<SceneRenderer>();
    if (!m_scene_renderer->initialize(*m_scene, window->get_width(), window->get_height()))
    {
        SE_LOG_ERROR("Failed to initialize the world renderer!"sv);
        return false;
    }

    m_scene->on_begin_play();
    return true;
}

void EditorEngine::shutdown()
{
    m_scene->on_end_play();

    m_scene_renderer->shutdown();
    m_scene_renderer.release();

    m_window_stack.clear_and_shrink();

    Renderer::shutdown();
    g_asset_manager->shutdown();

    g_editor_engine = nullptr;
    Engine::shutdown();
}

void EditorEngine::tick()
{
    for (OwnPtr<Window>& window : m_window_stack)
        window->pump_messages();

    for (auto it = m_window_stack.rbegin(); it != m_window_stack.rend(); --it)
    {
        OwnPtr<Window>& window = *it;
        if (window->should_close())
        {
            window.release();
            m_window_stack.remove(it);
        }
    }

    if (m_window_stack.is_empty())
    {
        m_is_running = false;
        SE_LOG_INFO("All windows have been closed. Exiting the main loop..."sv);
        return;
    }

    if (m_window_stack.first()->should_close())
    {
        m_window_stack.clear_and_shrink();
        m_is_running = false;
        SE_LOG_INFO("The primary window has been closed. Exiting the main loop..."sv);
        return;
    }

    Engine::tick();
    m_scene->on_update(0.0F);
    m_scene_renderer->render();
}

Window* EditorEngine::create_window()
{
    WindowInfo window_info = {};
    window_info.start_maximized = true;
    window_info.title = "ShooterEditor (Windows - Debug) | Untitled Scene*"sv;
    window_info.event_callback = EditorEngine::on_event;

    m_window_stack.add(Window::instantiate());
    OwnPtr<Window>& window = m_window_stack.last();

    if (!window->initialize(window_info))
    {
        m_window_stack.remove_last();
        return nullptr;
    }

    SE_LOG_INFO("Sucessfully created a window with the title '{}'."sv, window_info.title);
    return window.get();
}

Window* EditorEngine::find_window_by_native_handle(void* native_handle)
{
    //
    // If the last window in the stack doesn't have a valid native handle it means that the
    // Window class was instantiated, but the native window object hasn't been created yet.
    // On Windows, the function used to create the native window will post messages that
    // must be processed, but its native handle is still unknown.
    //
    if (m_window_stack.last()->get_native_handle() == nullptr)
        return m_window_stack.last().get();

    for (OwnPtr<Window>& window : m_window_stack)
    {
        if (window->get_native_handle() == native_handle)
            return window.get();
    }

    return nullptr;
}

#define SE_PROJECT_FILE_EXTENSION    ".seproject"sv
#define SE_PROJECT_DIRECTORY_CONTENT "Content"sv
#define SE_PROJECT_DIRECTORY_SOURCE  "Source"sv

String EditorEngine::get_project_file_filepath() const
{
    const String project_file = m_project_name + SE_PROJECT_FILE_EXTENSION;
    return StringBuilder::path_join({ m_project_root_directory.view(), project_file.view() });
}

String EditorEngine::get_project_content_directory() const
{
    return StringBuilder::path_join({ m_project_root_directory.view(), SE_PROJECT_DIRECTORY_CONTENT });
}

String EditorEngine::get_project_source_directory() const
{
    return StringBuilder::path_join({ m_project_root_directory.view(), SE_PROJECT_DIRECTORY_SOURCE });
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

            if (g_editor_engine->m_scene_renderer.is_valid())
            {
                // Propagate the event to the world renderer, if the instance was created.
                g_editor_engine->m_scene_renderer->on_resize(new_width, new_height);
            }

            break;
        }
    }
}

} // namespace SE
