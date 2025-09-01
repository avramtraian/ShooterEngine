// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Editor/EditorEngine/EditorEngine.h>
#include <Runtime/Application/Input.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Core/Time.h>
#include <Runtime/Renderer/RHI/RenderingDriver.h>
#include <Runtime/Renderer/RHI/RenderingSurface.h>
#include <Runtime/Renderer/ShaderLibrary.h>

namespace SE
{

EditorEngine* GEditorEngine = nullptr;

bool EditorEngine::Initialize()
{
    if (!Engine::Initialize())
    {
        return false;
    }

    m_EditorWindow = Window::Create(WindowInfo()
        .SetStartMode(WindowMode::Maximized)
#if SE_CONFIGURATION_EDITOR_DEBUG
        .SetTitle(VIEW("ShooterEditor - Windows 64-bit - EditorDebug | Untitled Project"))
#endif // SE_CONFIGURATION_EDITOR_DEBUG
#if SE_CONFIGURATION_EDITOR_DEVELOPMENT
        .SetTitle(VIEW("ShooterEditor - Windows 64-bit - EditorDevelopment | Untitled Project"))
#endif // SE_CONFIGURATION_EDITOR_DEVELOPMENT
    );
    if (!m_EditorWindow.IsValid())
    {
        // Failed to initialize the game window. As we don't have a window there is no point
        // in trying to continue the engine initialization process.
        SE_LOG_ERROR("Failed to initialize the primary game window! Aborting the initialization process.");
        return false;
    }
    SE_LOG_INFO("The primary game window was created successfully.");

    const bool inputInitializeResult = Input::Initialize();
    if (!inputInitializeResult)
    {
        // Failed to initialize the input system. As the user can't input any commands into the game,
        // there is no point in trying to continue the engine initialization.
        SE_LOG_ERROR("Failed to initialize the input system! Aborting the initialization process.");
        return false;
    }
    Input::AddSourceWindow(m_EditorWindow);
    SE_LOG_INFO("The input system was initialized successfully.");

    const bool renderingDriverInitializeResult = RenderingDriver::Initialize(RenderingDriverInfo()
        .SetBackend(RenderingDriverBackend::Vulkan)
    );
    if (!renderingDriverInitializeResult)
    {
        // Failed to initialize the rendering driver. As nothing can be rendered to the screen, there
        // is no point in trying to continue the engine initialization.
        SE_LOG_ERROR("Failed to initialize the rendering driver! Aborting the initialization process.");
        return false;
    }
    SE_LOG_INFO("The rendering driver was initialized successfully.");

    m_EditorRenderingSurface = g_RenderingDriver->CreateSurface(RenderingSurfaceInfo()
        .SetTargetWindow(m_EditorWindow)
        .SetSwapchainMinImageCount(3)
        .SetMaxFramesInFlight(2)
    );
    if (!m_EditorRenderingSurface.IsValid())
    {
        // Failed to create the rendering surface. As the user can't see anything on the screen without
        // it, there is no point in trying to continue the engine initialization.
        SE_LOG_ERROR("Failed to create the primary game rendering surface. Aborting the initialization process.");
        return false;
    }
    SE_LOG_INFO("The primary game rendering surface was created successfully.");

    const bool shaderLibraryInitializeResult = ShaderLibrary::Initialize();
    if (!shaderLibraryInitializeResult)
    {
        // The engine might not require to access any shaders, so this is not a critical error.
        // Don't exit the engine initialization process yet.
        SE_LOG_ERROR("Failed to initilize the shader library!");
    }

    return true;
}

void EditorEngine::Shutdown()
{
    SE_LOG_INFO("Shutting down the engine systems...");

    // NOTE(Traian): In order to safely destruct command lists we must ensure that they have finished execution.
    // Since currently we have no mechanism to track if a command list is still executing on a queue or not, and
    // it is the responsability of the appliction to ensure that a command list is not deleted until it finished
    // execution, we simmply block the current (main) thread until all operations on the GPU have finished.
    g_RenderingDriver->WaitForDeviceIdle();

    // Shutdown rendering subsystems and driver.
    ShaderLibrary::Shutdown();
    m_EditorRenderingSurface.Release();
    RenderingDriver::Shutdown();

    // Shutdown the input system.
    Input::Shutdown();

    // Destroy the editor window.
    m_EditorWindow.Release();

    Engine::Shutdown();
}

void EditorEngine::Execute()
{
    // Assume the first frame runs at 60FPS.
    float lastFrameDeltaTime = TimeDuration::FromMilliseconds(16).ToSeconds();

    while (m_EditorWindow.IsValid() && !m_EditorWindow->IsRequestedToClose())
    {
        Timer currentFrameTimer;
        currentFrameTimer.Start();

        m_EditorWindow->ProcessEventQueue();
        OnUpdate(lastFrameDeltaTime);

        currentFrameTimer.Stop();
        const TimeDuration frameDeltaTime = currentFrameTimer.GetElapsed();
        lastFrameDeltaTime = frameDeltaTime.ToSeconds();
    }
}

void EditorEngine::OnUpdate(float deltaTime)
{
    // Run pre-update events for engine systems.
    Input::OnPreUpdate(deltaTime);

    // Run main update events for engine systems.
    Input::OnUpdate(deltaTime);

    // Run post-update events for engine systems.
    Input::OnPostUpdate(deltaTime);
}

}
