/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/StringBuilder.h>
#include <Core/FileSystem/FileSystem.h>
#include <Core/Log.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/MatrixTransformations.h>
#include <EditorContext/EditorContext.h>
#include <EditorEngine.h>
#include <Engine/Application/Events/WindowEvents.h>
#include <Engine/Scene/Components/CameraComponent.h>
#include <Engine/Scene/Components/TransformComponent.h>
#include <Renderer/Renderer.h>
#include <Renderer/RendererAPI.h>
#include <Renderer/RenderingContext.h>

// ImGui includes.

#include <imgui.h>
#if SE_PLATFORM_WINDOWS
    #include <Core/Platform/Windows/WindowsHeaders.h>
    #include <backends/imgui_impl_win32.h>

// Forward declare message handler from imgui_impl_win32.cpp
// Extracted from https://github.com/ocornut/imgui/blob/master/examples/example_win32_directx11/main.cpp.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace SE
{

struct WindowNativeEventData
{
    HWND window_handle;
    UINT message;
    WPARAM w_param;
    LPARAM l_param;
};

} // namespace SE

#endif // SE_PLATFORM_WINDOWS
#if SE_RENDERER_API_SUPPORTED_D3D11
    #include <backends/imgui_impl_dx11.h>
#endif // SE_RENDERER_API_SUPPORTED_D3D11

namespace SE
{

bool EditorContext::pre_initialize()
{
    // The editor executable always has the working directory set to the engine root directory.
    m_engine_root_directory = FileSystem::get_working_directory();

    // TODO: Allow the user to specify the project to be opened using a command line argument.
    m_project_name = "ExampleProject"sv;
    m_project_root_directory = StringBuilder::path_join({ m_engine_root_directory.view(), "Content/ExampleProject"sv });

    return true;
}

bool EditorContext::initialize()
{
    // Create the editor window.
    WindowDescription window_description = {};
    window_description.start_mode = WindowMode::Maximized;
    window_description.event_callback = EditorEngine::static_on_event;
    window_description.native_event_callback = [](const WindowNativeEventData* data) -> uintptr
    {
#if SE_PLATFORM_WINDOWS
        return ImGui_ImplWin32_WndProcHandler(data->window_handle, data->message, data->w_param, data->l_param);
#endif // SE_PLATFORM_WINDOWS
    };

    m_window = Window::create(window_description);
    if (!m_window.is_valid())
        return false;

    // Create the rendering context for the window.
    if (!Renderer::create_context_for_window(m_window.get()))
        return false;
    RenderingContext* context = Renderer::get_context_for_window(m_window.get());
    Renderer::set_active_context(context);

    // Initialize the scene.
    m_active_scene = Scene::create();

    // Initialize the component registry.
    m_component_registry.initialize();

    // Create the scene framebuffer and scene renderer.
    FramebufferDescription scene_framebuffer_description = {};
    scene_framebuffer_description.width = 1200;
    scene_framebuffer_description.height = 800;
    auto& color_scene_framebuffer_attachment = scene_framebuffer_description.attachments.emplace();
    color_scene_framebuffer_attachment.format = ImageFormat::RGBA8;
    color_scene_framebuffer_attachment.use_as_input_texture = true;
    m_scene_framebuffer = Framebuffer::create(scene_framebuffer_description);

    m_scene_renderer = create_own<SceneRenderer>();
    m_scene_renderer->initialize(*m_active_scene, m_scene_framebuffer);

    // Initialize the editor camera.
    // clang-format off
    m_editor_camera.invalidate(
        Vector3(0, 0, -3),
        Vector3(0, 0, 0),
        m_scene_framebuffer->get_width(),
        m_scene_framebuffer->get_height(),
        Math::radians(70.0F),
        0.001F, 10000.0F
    );
    // clang-format on

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#if SE_RENDERER_API_SUPPORTED_D3D11
    if (get_current_renderer_api() == RendererAPI::D3D11)
    {
        ID3D11Device* device = static_cast<ID3D11Device*>(Renderer::get_device().d3d11.device_handle);
        ID3D11DeviceContext* device_context = static_cast<ID3D11DeviceContext*>(Renderer::get_device().d3d11.device_context_handle);
        ImGui_ImplWin32_Init(m_window->get_native_handle());
        ImGui_ImplDX11_Init(device, device_context);
    }
#endif // SE_RENDERER_API_SUPPORTED_D3D11

    ShaderDescription imgui_shader_description = {};
    imgui_shader_description.debug_name = "ImGuiShader"sv;
    // NOTE: The ImGui implementation will actually bind its own shaders when invoking the render command.
    // This shader is created just as a mean to create the render pass that will contain the ImGui rendering,
    // thus it doesn't actually have any stages.
    m_imgui_shader = Shader::create(imgui_shader_description);

    PipelineDescription imgui_pipeline_description = {};
    imgui_pipeline_description.shader = m_imgui_shader;
    // NOTE: The ImGui implementation will actually bind its own input layout when invoking the render command.
    // This pipeline is created just as a mean to create the rendre pass that will contain the ImGui rendering,
    // thus it doesn't actually have a set of vertex attributes.
    m_imgui_pipeline = Pipeline::create(imgui_pipeline_description);

    RenderPassDescription imgui_render_pass_description;
    imgui_render_pass_description.pipeline = m_imgui_pipeline;
    imgui_render_pass_description.target_framebuffer = Renderer::get_swapchain_framebuffer(Renderer::get_context_for_window(m_window.get()));
    for (u32 attachment_index = 0; attachment_index < imgui_render_pass_description.target_framebuffer->get_attachment_count(); ++attachment_index)
    {
        RenderPassAttachmentDescription attachment = {};
        attachment.load_operation = RenderPassAttachmentLoadOperation::Clear;
        attachment.clear_color = Color4(0, 0, 0);
        imgui_render_pass_description.target_framebuffer_attachments.add(attachment);
    }

    m_imgui_render_pass = RenderPass::create(imgui_render_pass_description);

    return true;
}

bool EditorContext::post_initialize()
{
    m_content_browser_panel.initialize();

    m_entity_inspector_panel.initialize();
    m_entity_inspector_panel.set_scene_context(m_active_scene.get());
    m_entity_inspector_panel.set_component_registry_context(&m_component_registry);

    m_scene_hierarchy_panel.initialize();
    m_scene_hierarchy_panel.set_scene_context(m_active_scene.get());
    m_scene_hierarchy_panel.add_on_selection_changed_callback(
        [this](Optional<UUID> selected_entity_uuid)
        {
            if (selected_entity_uuid.has_value())
                m_entity_inspector_panel.set_entity_uuid_context(selected_entity_uuid.value());
            else
                m_entity_inspector_panel.clear_entity_uuid_context();
        }
    );

    m_viewport_panel.initialize();
    m_viewport_panel.set_scene_framebuffer_context(m_scene_framebuffer);
    m_viewport_panel.add_on_viewport_resized_callback(
        [this](u32 viewport_width, u32 viewport_height)
        {
            if (viewport_width == 0 || viewport_height == 0)
                return;

            m_scene_framebuffer->invalidate(viewport_width, viewport_height);
            m_scene_renderer->on_resize(viewport_width, viewport_height);
            m_editor_camera.set_viewport_size(viewport_width, viewport_height);
        }
    );

    m_toolbar_panel.initialize();
    m_toolbar_panel.set_editor_camera_controller_context(&m_editor_camera.get_controller());
    m_toolbar_panel.add_on_scene_play_changed_callback(
        [this](ScenePlayState old_scene_play_state, ScenePlayState new_scene_play_state)
        {
            if (new_scene_play_state == ScenePlayState::Unknown)
                return;
            SE_ASSERT(new_scene_play_state != old_scene_play_state);

            switch (new_scene_play_state)
            {
                case ScenePlayState::Play:
                {
                    if (old_scene_play_state == ScenePlayState::PlayPaused)
                        on_scene_unpause();
                    else
                        on_scene_play();
                }
                break;

                case ScenePlayState::PlayPaused:
                {
                    if (old_scene_play_state == ScenePlayState::Play)
                        on_scene_pause();
                }
                break;

                case ScenePlayState::Edit:
                {
                    on_scene_stop();
                }
                break;
            }
        }
    );
    m_toolbar_panel.add_on_scene_camera_mode_changed_callback(
        [this](SceneCameraMode old_camera_mode, SceneCameraMode new_camera_mode)
        {
            if (new_camera_mode == SceneCameraMode::Unknown)
                return;
            SE_ASSERT(new_camera_mode != old_camera_mode);

            switch (new_camera_mode)
            {
                case SceneCameraMode::Editor: on_scene_camera_mode_set_to_editor(); break;
                case SceneCameraMode::Game: on_scene_camera_mode_set_to_game(); break;
            }
        }
    );

    return true;
}

void EditorContext::shutdown()
{
    if (is_scene_in_play_state())
    {
        // End the scene play session before shutting down the editor context.
        m_active_scene->on_end_play();
    }

    m_content_browser_panel.shutdown();
    m_entity_inspector_panel.shutdown();
    m_scene_hierarchy_panel.shutdown();
    m_viewport_panel.shutdown();
    m_toolbar_panel.shutdown();

    m_imgui_render_pass.release();
    m_imgui_pipeline.release();
    m_imgui_shader.release();

#if SE_RENDERER_API_SUPPORTED_D3D11
    if (get_current_renderer_api() == RendererAPI::D3D11)
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
    }
#endif // SE_RENDERER_API_SUPPORTED_D3D11

    ImGui::DestroyContext();

    m_scene_renderer.release();
    m_active_scene.release();
    m_component_registry.shutdown();

    // Destroy the window.
    Renderer::destroy_context_for_window(m_window.get());
    m_window.release();
}

void EditorContext::on_pre_update(float delta_time)
{
    // Process the window message queue.
    m_window->pump_messages();
    if (m_window->should_close())
        g_editor_engine->exit();
}

void EditorContext::on_update(float delta_time)
{
    Renderer::begin_frame();

    // Update the editor.
    on_update_logic(delta_time);

    // Prepare ImGui for rendering.
    Renderer::begin_render_pass(m_imgui_render_pass);
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();

    // Create a dockspace over the entire editor window surface.
    ImGui::DockSpaceOverViewport();

    // Render editor interface using ImGui.
    on_render_imgui();

    // Submit the ImGui rendering data to the GPU.
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    Renderer::end_render_pass();

    Renderer::end_frame();
}

void EditorContext::on_event(const Event& in_event)
{
    switch (in_event.get_type())
    {
        case EventType::WindowResized:
        {
            const WindowResizedEvent& e = static_cast<const WindowResizedEvent&>(in_event);
            m_scene_renderer->on_resize(e.get_client_width(), e.get_client_height());
        }
        break;
    }
}

BuildConfiguration EditorContext::get_current_build_configuration() const
{
#if SE_CONFIGURATION_TARGET_EDITOR
    #if SE_CONFIGURATION_DEBUG
    return BuildConfiguration::EditorDebug;
    #endif // SE_CONFIGURATION_DEBUG
    #if SE_CONFIGURATION_DEVELOPMENT
    return BuildConfiguration::EditorDevelopment;
    #endif // SE_CONFIGURATION_DEVELOPMENT
#endif // SE_CONFIGURATION_TARGET_EDITOR

#if SE_CONFIGURATION_TARGET_GAME
    #if SE_CONFIGURATION_DEBUG
    return BuildConfiguration::GameDebug;
    #endif // SE_CONFIGURATION_DEBUG
    #if SE_CONFIGURATION_DEVELOPMENT
    return BuildConfiguration::GameDevelopment;
    #endif // SE_CONFIGURATION_DEVELOPMENT
    #if SE_CONFIGURATION_SHIPPING
    return BuildConfiguration::GameShipping;
    #endif // SE_CONFIGURATION_SHIPPING
#endif // SE_CONFIGURATION_TARGET_GAME
}

String EditorContext::get_project_source_directory() const
{
    const String project_source_directory = StringBuilder::join({ m_project_root_directory.view(), "/Source"sv });
    return project_source_directory;
}

String EditorContext::get_project_content_directory() const
{
    const String project_content_directory = StringBuilder::join({ m_project_root_directory.view(), "/Content"sv });
    return project_content_directory;
}

String EditorContext::get_project_binaries_directory(BuildConfiguration build_configuration) const
{
    StringView relative_binary_filepath;
    switch (build_configuration)
    {
        case BuildConfiguration::EditorDebug: relative_binary_filepath = "Binaries/EditorDebug"sv; break;
        case BuildConfiguration::EditorDevelopment: relative_binary_filepath = "Binaries/EditorDevelopment"sv; break;
        case BuildConfiguration::GameDebug: relative_binary_filepath = "Binaries/GameDebug"sv; break;
        case BuildConfiguration::GameDevelopment: relative_binary_filepath = "Binaries/GameDevelopment"sv; break;
        case BuildConfiguration::GameShipping: relative_binary_filepath = "Binaries/GameShipping"sv; break;
    }

    const String project_binaries_directory = StringBuilder::join({ m_project_root_directory.view(), relative_binary_filepath });
    return project_binaries_directory;
}

void EditorContext::on_update_logic(float delta_time)
{
    if (is_scene_in_play_state())
    {
        if (get_scene_play_state() == ScenePlayState::Play)
        {
            // If the scene play state is set to `Play` invoke the scene `on_update` function.
            // This will effectively update all game-related code (including engine systems).
            m_active_scene->on_update(delta_time);
        }
    }

    Matrix4 view_projection_matrix = Matrix4::identity();
    if (get_scene_camera_mode() == SceneCameraMode::Editor)
    {
        m_editor_camera.on_update(delta_time);
        view_projection_matrix = m_editor_camera.get_view_projection_matrix();
    }
    else if (get_scene_camera_mode() == SceneCameraMode::Game)
    {
        Entity* entity = m_active_scene->get_primary_camera_entity();
        if (entity == nullptr && is_scene_in_edit_state())
        {
            const UUID entity_uuid = m_active_scene->find_primary_camera_entity();
            if (entity_uuid != UUID::invalid())
                entity = m_active_scene->get_entity_from_uuid(entity_uuid);
        }

        if (entity == nullptr || !entity->has_component<TransformComponent>())
        {
            // The primary camera entity can't be used to calculate a valid view projection matrix,
            // so we use the default editor camera instead. This behaviour should be displayed to the user.
            view_projection_matrix = m_editor_camera.get_view_projection_matrix();
        }
        else
        {
            const TransformComponent& tc = entity->get_component<TransformComponent>();
            const CameraComponent& cc = entity->get_component<CameraComponent>();

            const Matrix4 inverse_view_matrix = Matrix4::rotate(tc.rotation()) * Matrix4::translate(tc.translation());
            const float aspect_ratio = static_cast<float>(m_scene_framebuffer->get_width()) / static_cast<float>(m_scene_framebuffer->get_height());
            const Matrix4 projection_matrix = cc.get_projection_matrix(aspect_ratio);

            view_projection_matrix = Matrix4::inverse(inverse_view_matrix) * projection_matrix;
        }
    }

    // Render the scene.
    // TODO: Use the view projection matrix of the primary camera entity when the scene
    // play state is set to `Play`.
    m_scene_renderer->render(view_projection_matrix);

    m_content_browser_panel.on_update(delta_time);
    m_entity_inspector_panel.on_update(delta_time);
    m_scene_hierarchy_panel.on_update(delta_time);
    m_viewport_panel.on_update(delta_time);
    m_toolbar_panel.on_update(delta_time);
}

void EditorContext::on_render_imgui()
{
    m_content_browser_panel.on_render_imgui();
    m_entity_inspector_panel.on_render_imgui();
    m_scene_hierarchy_panel.on_render_imgui();
    m_viewport_panel.on_render_imgui();
    m_toolbar_panel.on_render_imgui();
}

void EditorContext::on_scene_play()
{
    m_toolbar_panel.set_scene_camera_mode(SceneCameraMode::Game);
    m_active_scene->on_begin_play();
}

void EditorContext::on_scene_stop()
{
    m_active_scene->on_end_play();
    m_toolbar_panel.set_scene_camera_mode(SceneCameraMode::Editor);
}

void EditorContext::on_scene_pause()
{}

void EditorContext::on_scene_unpause()
{}

void EditorContext::on_scene_camera_mode_set_to_editor()
{}

void EditorContext::on_scene_camera_mode_set_to_game()
{
    if (m_active_scene->find_primary_camera_entity() == UUID::invalid())
    {
        // There is no primary camera entity in the scene, thus we can't switch to the
        // game camera (as it doesn't exist).
        SE_LOG_INFO("No primary camera entity was found in the scene. Falling back to the editor camera.");
        m_toolbar_panel.set_scene_camera_mode(SceneCameraMode::Editor);
    }
}

} // namespace SE
