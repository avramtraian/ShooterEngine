// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

enum class RenderingDriverBackend : uint8
{
    None = 0,
    Vulkan,
    MaxEnumValue,
};

struct RenderingDriverInfo
{
public:
    RenderingDriverBackend Backend { RenderingDriverBackend::None };

public:
    inline RenderingDriverInfo& SetBackend(RenderingDriverBackend backend) { Backend = backend; return *this; }
};

class RenderingDriver
{
    SE_MAKE_SINGLETON_CLASS(RenderingDriver);

public:
    static bool Initialize(const RenderingDriverInfo& info);
    static void Shutdown();

private:
    virtual bool InitializeBackend(const RenderingDriverInfo& info) = 0;
    virtual void ShutdownBackend() = 0;
};

SHOOTER_API extern RenderingDriver* g_RenderingDriver;

}
