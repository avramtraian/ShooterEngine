// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Input.h>
#include <Runtime/Core/Containers/StringView.h>
#include <Runtime/Core/Log.h>
#include <Runtime/Engine/GameEngine.h>
#include <Runtime/Renderer/RHI/RenderingDriver.h>
#include <Runtime/Renderer/RHI/RenderingSurface.h>
#include <Runtime/Renderer/ShaderLibrary.h>

namespace SE
{

SHOOTER_API GameEngine* g_GameEngine = nullptr;

bool GameEngine::Initialize()
{
    if (!InitializeCoreSystems())
    {
        /* There is no point of trying to continue the initialization process if critical
         * systems were not able to be initialized. */
        return false;
    }
    SE_LOG_INFO("All core systems were initialized successfully.");

    m_GameWindow = Window::Create(WindowInfo()
        .SetStartMode(WindowMode::Maximized)
        .SetTitle(VIEW("Shooter Game"))
    );
    if (!m_GameWindow.IsValid())
    {
        /* Failed to initialize the game window. As we don't have a window there is no point
         * in trying to continue the engine initialization process. */
        SE_LOG_ERROR("Failed to initialize the primary game window! Aborting the initialization process.");
        return false;
    }
    m_GameWindow->AddEventCallback(EventType::WindowResized, [](const Window&, const Event& event) { g_GameEngine->OnGameWindowResized((const WindowResizedEvent&)event); });
    SE_LOG_INFO("The primary game window was created successfully.");

    const bool inputInitializeResult = Input::Initialize(InputInfo()
        .AddSourceWindow(m_GameWindow.Get())
    );
    if (!inputInitializeResult)
    {
        /* Failed to initialize the input system. As the user can't input any commands into the game,
         * there is no point in trying to continue the engine initialization. */
        SE_LOG_ERROR("Failed to initialize the input system! Aborting the initialization process.");
        return false;
    }
    SE_LOG_INFO("The input system was initialized successfully.");
    
    const bool renderingDriverInitializeResult = RenderingDriver::Initialize(RenderingDriverInfo()
        .SetBackend(RenderingDriverBackend::Vulkan)
    );
    if (!renderingDriverInitializeResult)
    {
        /* Failed to initialize the rendering driver. As nothing can be rendered to the screen, there
         * is no point in trying to continue the engine initialization. */
        SE_LOG_ERROR("Failed to initialize the rendering driver! Aborting the initialization process.");
        return false;
    }
    SE_LOG_INFO("The rendering driver was initialized successfully.");

    m_GameRenderingSurface = g_RenderingDriver->CreateSurface(RenderingSurfaceInfo()
        .SetOwningWindow(m_GameWindow.Get())
        .SetSwapchainMinImageCount(3)
        .SetMaxFramesInFlight(3)
        .SetEnableVSync(true)
    );
    if (!m_GameRenderingSurface)
    {
        /* Failed to create the rendering surface. As the user can't see anything on the screen without
         * it, there is no point in trying to continue the engine initialization. */
        SE_LOG_ERROR("Failed to create the primary game rendering surface. Aborting the initialization process.");
        return false;
    }
    SE_LOG_INFO("The primary game rendering surface was created successfully.");

    const bool shaderLibraryInitializeResult = ShaderLibrary::Initialize();
    if (!shaderLibraryInitializeResult)
    {
        /* The engine might not require to access any shaders, so this is not a critical error.
         * Don't exit the engine initialization process yet. */
        SE_LOG_ERROR("Failed to initilize the shader library!");
    }

    return true;
}

void GameEngine::Shutdown()
{
    SE_LOG_INFO("Shutting down the engine systems...");

    // Shutdown rendering subsystems and driver.
    g_RenderingDriver->WaitForDeviceIdle();
    ShaderLibrary::Shutdown();
    m_GameRenderingSurface.reset();
    RenderingDriver::Shutdown();
    
    // Shutdown the input system.
    Input::Shutdown();

    /* Destroy the game window. */
    m_GameWindow.Release();

    SE_LOG_INFO("Shutting down the core systems...");
    ShutdownCoreSystems();
}

void GameEngine::Execute()
{
    while (!m_GameWindow->IsRequestedToClose())
    {
        m_GameWindow->PumpMessages();
        OnUpdate();
    }
}

void GameEngine::OnUpdate()
{
    Input::OnUpdate();
    Input::OnPostUpdate();
}

void GameEngine::OnGameWindowResized(const WindowResizedEvent& resizedEvent)
{
    if (resizedEvent.GetNewSizeX() > 0 && resizedEvent.GetNewSizeY() > 0)
    {
        /* NOTE(Traian): Invalidating a rendering surface that targets a zero-sized window might
         * cause undefined behaviour and trigger the validation layers. */
        
        if (m_GameRenderingSurface->GetSurfaceSizeX() != resizedEvent.GetNewSizeX() || m_GameRenderingSurface->GetSurfaceSizeY() != resizedEvent.GetNewSizeY())
        {
            /* NOTE(Traian): Only recreate the rendering surface if the new window size doesn't match
             * the size of the existing rendering surface. */
            m_GameRenderingSurface->Invalidate();
        }
    }
}

}
