/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/StringBuilder.h>
#include <Core/FileSystem/FileSystem.h>
#include <EditorContext/EditorContext.h>
#include <EditorEngine.h>
#include <Engine/Application/WindowEvent.h>
#include <Engine/Scene/Components/SpriteRendererComponent.h>
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
    m_window = Window::instantiate();
    WindowInfo window_info = {};
    window_info.start_maximized = true;
    window_info.event_callback = EditorContext::window_event_handler;
    window_info.native_event_callback = [](void* native_event_data)
    {
#if SE_PLATFORM_WINDOWS
        struct NativeEventData
        {
            HWND window_handle;
            UINT message;
            WPARAM w_param;
            LPARAM l_param;
        };

        NativeEventData* event_data = static_cast<NativeEventData*>(native_event_data);
        ImGui_ImplWin32_WndProcHandler(event_data->window_handle, event_data->message, event_data->w_param, event_data->l_param);
#endif // SE_PLATFORM_WINDOWS
    };

    if (!m_window->initialize(window_info))
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
    const float editor_camera_aspect_ratio = static_cast<float>(m_scene_framebuffer->get_width()) / static_cast<float>(m_scene_framebuffer->get_height());
    m_editor_camera.invalidate(Vector3(0, 0, -3), Vector3(0, 0, 0), Math::radians(70.0F), editor_camera_aspect_ratio, 0.001F, 10000.0F);

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
            m_scene_framebuffer->invalidate(viewport_width, viewport_height);
            m_scene_renderer->on_resize(viewport_width, viewport_height);

            if (viewport_width != 0 && viewport_height != 0)
                m_editor_camera.set_aspect_ratio(static_cast<float>(viewport_width) / static_cast<float>(viewport_height));
        }
    );

    Entity* entity = m_active_scene->create_entity();
    entity->add_component<TransformComponent>();
    entity->add_component<SpriteRendererComponent>();

    m_active_scene->create_entity();
    m_active_scene->create_entity();
    m_active_scene->create_entity();
    m_active_scene->create_entity();

    return true;
}

void EditorContext::shutdown()
{
    m_content_browser_panel.shutdown();
    m_entity_inspector_panel.shutdown();
    m_scene_hierarchy_panel.shutdown();
    m_viewport_panel.shutdown();

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

void EditorContext::on_update(float delta_time)
{
    // Process the window message queue.
    m_window->pump_messages();
    if (m_window->should_close())
        g_editor_engine->exit();

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
            Renderer::on_resize(e.get_client_width(), e.get_client_height());
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

void EditorContext::window_event_handler(const Event& in_event)
{
    // Forward the event to the non-static `on_event` callback.
    g_editor_engine->context().on_event(in_event);
}

void EditorContext::on_update_logic(float delta_time)
{
    // Render the scene.
    m_scene_renderer->render();

    m_content_browser_panel.on_update(delta_time);
    m_entity_inspector_panel.on_update(delta_time);
    m_scene_hierarchy_panel.on_update(delta_time);
    m_viewport_panel.on_update(delta_time);
}

void EditorContext::on_render_imgui()
{
    m_content_browser_panel.on_render_imgui();
    m_entity_inspector_panel.on_render_imgui();
    m_scene_hierarchy_panel.on_render_imgui();
    m_viewport_panel.on_render_imgui();
}

} // namespace SE
