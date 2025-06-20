// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/RenderingDriver.h>

#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>

namespace SE
{

SHOOTER_API RenderingDriver* g_RenderingDriver;

bool RenderingDriver::Initialize(const RenderingDriverInfo& info)
{
    if (g_RenderingDriver)
        return false;

    switch (info.Backend)
    {
        case RenderingDriverBackend::None:   g_RenderingDriver = nullptr;                     break;
        case RenderingDriverBackend::Vulkan: g_RenderingDriver = new VulkanRenderingDriver(); break;
    }

    if (g_RenderingDriver == nullptr)
        return false;

    return g_RenderingDriver->InitializeBackend(info);
}

void RenderingDriver::Shutdown()
{
    if (!g_RenderingDriver)
        return;

    g_RenderingDriver->ShutdownBackend();
    delete g_RenderingDriver;
    g_RenderingDriver = nullptr;
}

}
