// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

/* RHI classes forward declarations. */
class RenderingDriver;
class RenderingSurface;

/* RHI info structs forward declarations. */
struct RenderingDriverInfo;
struct RenderingSurfaceInfo;

}

#define SE_MAKE_RENDERER_RHI_INTERFACE(type_name) \
    private:                                      \
        SE_MAKE_NONCOPYABLE(type_name);           \
        SE_MAKE_NONMOVABLE(type_name);            \
    protected:                                    \
        type_name() = default;                    \
    public:                                       \
        virtual ~type_name() = default;           \
        friend class RenderingDriver;             \
        friend class VulkanRenderingDriver;       \
    private:
