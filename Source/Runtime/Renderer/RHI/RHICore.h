// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

/* RHI classes forward declarations. */
class CommandList;
class IndexBuffer;
class GraphicsState;
class Pipeline;
class RenderPass;
class RenderingDriver;
class RenderingSurface;
class Shader;
class Texture2D;
class VertexBuffer;

/* RHI info structs forward declarations. */
struct CommandListInfo;
struct IndexBufferInfo;
struct PipelineInfo;
struct RenderPassInfo;
struct RenderingDriverInfo;
struct RenderingSurfaceInfo;
struct ShaderInfo;
struct Texture2DInfo;
struct VertexBufferInfo;

/* Structures used for command dispatching. */
struct CommandListExecuteInfo;

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
