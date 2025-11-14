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
#if SE_CONFIGURATION_DEBUG
                                        .SetTitle(VIEW("ShooterEditor - Windows 64-bit - EditorDebug | Untitled Project"))
#endif // SE_CONFIGURATION_DEBUG
#if SE_CONFIGURATION_DEVELOPMENT
                                        .SetTitle(VIEW("ShooterEditor - Windows 64-bit - EditorDevelopment | Untitled Project"))
#endif // SE_CONFIGURATION_DEVELOPMENT
    );
    if (!m_EditorWindow.IsValid())
    {
        // Failed to initialize the game window. As we don't have a window there is no point
        // in trying to continue the engine initialization process.
        SE_LOG_ERROR("Failed to initialize the primary game window! Aborting the initialization process.");
        return false;
    }
    m_EditorWindow->GetOnWindowResizedDelegate().AddRaw([](StrongRefPtr<Window> window, uint32 newWindowSizeX, uint32 newWindowSizeY)
                                                        { GEditorEngine->OnWindowResized(Move(window), newWindowSizeX, newWindowSizeY); });
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

    const bool renderingDriverInitializeResult = RenderingDriver::Initialize(RenderingDriverInfo().SetBackend(RenderingDriverBackend::Vulkan));
    if (!renderingDriverInitializeResult)
    {
        // Failed to initialize the rendering driver. As nothing can be rendered to the screen, there
        // is no point in trying to continue the engine initialization.
        SE_LOG_ERROR("Failed to initialize the rendering driver! Aborting the initialization process.");
        return false;
    }
    SE_LOG_INFO("The rendering driver was initialized successfully.");

    m_EditorRenderingSurface =
        g_RenderingDriver->CreateSurface(RenderingSurfaceInfo().SetTargetWindow(m_EditorWindow).SetSwapchainMinImageCount(3).SetMaxFramesInFlight(2));
    if (!m_EditorRenderingSurface.IsValid())
    {
        // Failed to create the rendering surface. As the user can't see anything on the screen without
        // it, there is no point in trying to continue the engine initialization.
        SE_LOG_ERROR("Failed to create the primary editor rendering surface. Aborting the initialization process.");
        return false;
    }
    SE_LOG_INFO("The primary editor rendering surface was created successfully.");

    const bool shaderLibraryInitializeResult = ShaderLibrary::Initialize();
    if (!shaderLibraryInitializeResult)
    {
        // The engine might not require to access any shaders, so this is not a critical error.
        // Don't exit the engine initialization process yet.
        SE_LOG_ERROR("Failed to initialize the shader library!");
        return false;
    }

    const uint32 maxFramesInFlight = m_EditorRenderingSurface->GetMaxFramesInFlight();
    m_EditorCommandLists.EnsureCapacity(maxFramesInFlight);
    for (uint32 commandListIndex = 0; commandListIndex < maxFramesInFlight; ++commandListIndex)
    {
        RefPtr<CommandList> commandList = g_RenderingDriver->CreateCommandList(CommandListInfo().SetFamily(CommandListFamily::Graphics));
        m_EditorCommandLists.Add(Move(commandList));
    }

    m_ActiveScene         = CreateRef<Scene>();
    m_ActiveSceneRenderer = SceneRenderer::Create(SceneRendererInfo()
                                                      .SetSceneContext(m_ActiveScene)
                                                      .SetMaxFramesInFlight(maxFramesInFlight)
                                                      .SetRenderToSwapchainTarget(true)
                                                      .SetRenderTargetFormat(m_EditorRenderingSurface->GetSurfaceFormat()));

    return true;
}

void EditorEngine::Shutdown()
{
    SE_LOG_INFO("Shutting down the engine systems...");
    m_ActiveScene.Release();

    // NOTE(Traian): In order to safely destruct command lists we must ensure that they have finished execution.
    // Since currently we have no mechanism to track if a command list is still executing on a queue or not, and
    // it is the responsibility of the appliction to ensure that a command list is not deleted until it finished
    // execution, we simply block the current (main) thread until all operations on the GPU have finished.
    g_RenderingDriver->WaitForDeviceIdle();

    m_ActiveSceneRenderer.Release();
    m_EditorRenderingSurface.Release();
    m_EditorCommandLists.ClearAndShrink();

    // Shutdown rendering subsystems and driver.
    ShaderLibrary::Shutdown();
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

    m_ActiveScene->OnBeginPlay();

    while (m_EditorWindow.IsValid() && !m_EditorWindow->IsRequestedToClose())
    {
        Timer currentFrameTimer;
        currentFrameTimer.Start();

        m_EditorWindow->ProcessEventQueue();
        OnUpdate(lastFrameDeltaTime);

        currentFrameTimer.Stop();
        const TimeDuration frameDeltaTime = currentFrameTimer.GetElapsed();
        lastFrameDeltaTime                = frameDeltaTime.ToSeconds();
    }

    m_ActiveScene->OnEndPlay();
}

void EditorEngine::OnUpdate(float deltaTime)
{
    // Run pre-update events for engine systems.
    Input::OnPreUpdate(deltaTime);

    // Run main update events for engine systems.
    Input::OnUpdate(deltaTime);

    // Update the scene.
    m_ActiveScene->OnUpdate(deltaTime);

    // Render the scene.
    if (m_EditorWindow->GetSizeX() > 0 && m_EditorWindow->GetSizeY() > 0)
    {
        // Begin the frame.
        m_EditorRenderingSurface->BeginFrame();
        const uint32        currentFrameIndex = m_EditorRenderingSurface->GetCurrentFrameIndex();
        RefPtr<CommandList> commandList       = m_EditorCommandLists[currentFrameIndex];
        commandList->Begin();

        // Render the scene.
        m_ActiveSceneRenderer->Render(SceneRendererRenderInfo()
                                          .SetFrameIndex(currentFrameIndex)
                                          .SetRenderTarget(m_EditorRenderingSurface->GetCurrentSurfaceTexture2D())
                                          .SetCommandList(commandList)
                                          .SetImageAvailableSemaphore(m_EditorRenderingSurface->GetImageAvailableSemaphore())
                                          .SetRenderFinishedSemaphore(m_EditorRenderingSurface->GetRenderFinishedSemaphore())
                                          .SetRenderFinishedFence(m_EditorRenderingSurface->GetRenderFinishedFence()));

        // End the frame.
        commandList->End();
        g_RenderingDriver->ExecuteCommandList(
            commandList, CommandListExecuteInfo()
                             .AddWaitSemaphore(m_EditorRenderingSurface->GetImageAvailableSemaphore(), PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
                             .AddSignalSemaphore(m_EditorRenderingSurface->GetRenderFinishedSemaphore())
                             .SetSignalFence(m_EditorRenderingSurface->GetRenderFinishedFence()));
        m_EditorRenderingSurface->EndFrame(true);
    }

    // Run post-update events for engine systems.
    Input::OnPostUpdate(deltaTime);
}

void EditorEngine::OnWindowResized(StrongRefPtr<Window> window, uint32 newWindowSizeX, uint32 newWindowSizeY)
{
    // Resize the rendering surface when the window is resized.
    if (newWindowSizeX > 0 && newWindowSizeY > 0)
        m_EditorRenderingSurface->Invalidate();
}

} // namespace SE
