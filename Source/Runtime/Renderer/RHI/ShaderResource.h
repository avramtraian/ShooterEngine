// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>

#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace SE
{

using ShaderResourceCallbackID = uint32;
constexpr ShaderResourceCallbackID INVALID_SHADER_RESOURCE_CALLBACK_ID = 0;
using PFN_ShaderResourcePreDestroyCallback = std::function<void(ShaderResource&)>;

class ShaderResourcePreDestroyCallback
{
    SE_MAKE_NONCOPYABLE(ShaderResourcePreDestroyCallback);

public:
    ShaderResourcePreDestroyCallback() = default;
    FORCEINLINE ~ShaderResourcePreDestroyCallback() { Release(); }

    SHOOTER_API ShaderResourcePreDestroyCallback(const RefPtr<ShaderResource>& resource, PFN_ShaderResourcePreDestroyCallback callback);
    SHOOTER_API ShaderResourcePreDestroyCallback(const RefPtr<ShaderResource>& resource, ShaderResourceCallbackID callbackID);

    SHOOTER_API void Set(const RefPtr<ShaderResource>& resource, PFN_ShaderResourcePreDestroyCallback callback);
    SHOOTER_API void Set(const RefPtr<ShaderResource>& resource, ShaderResourceCallbackID callbackID);

    SHOOTER_API void Release();

public:
    FORCEINLINE ShaderResourcePreDestroyCallback(ShaderResourcePreDestroyCallback&& other) noexcept
        : m_Resource(other.m_Resource)
        , m_CallbackID(other.m_CallbackID)
    {
        other.m_Resource.Release();
        other.m_CallbackID = INVALID_SHADER_RESOURCE_CALLBACK_ID;
    }

    FORCEINLINE ShaderResourcePreDestroyCallback& operator=(ShaderResourcePreDestroyCallback&& other) noexcept
    {
        // Handle self-assignment case.
        if (this == &other)
            return *this;

        Release();

        m_Resource = other.m_Resource;
        m_CallbackID = other.m_CallbackID;
        other.m_Resource.Release();
        other.m_CallbackID = INVALID_SHADER_RESOURCE_CALLBACK_ID;

        return *this;
    }

private:
    WeakRefPtr<ShaderResource> m_Resource;
    ShaderResourceCallbackID m_CallbackID { INVALID_SHADER_RESOURCE_CALLBACK_ID };
};

class ShaderResource : public RefCounted
{
public:
    ShaderResource() = default;
    virtual ~ShaderResource() override = default;

    NODISCARD SHOOTER_API ShaderResourceCallbackID AddPreDestroyCallbackID(PFN_ShaderResourcePreDestroyCallback callback);
    SHOOTER_API void RemovePreDestroyCallbackID(ShaderResourceCallbackID callbackID);

    NODISCARD FORCEINLINE ShaderResourcePreDestroyCallback AddPreDestroyCallback(PFN_ShaderResourcePreDestroyCallback callback)
    {
        ShaderResourcePreDestroyCallback scopedCallback;
        const ShaderResourceCallbackID callbackID = AddPreDestroyCallbackID(std::move(callback));
        scopedCallback.Set(AdoptRef(this), callbackID);
        return scopedCallback;
    }

protected:
    SHOOTER_API void DispatchPreDestroyCallbacks();

private:
    std::unordered_map<ShaderResourceCallbackID, PFN_ShaderResourcePreDestroyCallback> m_PreDestroyCallbacks;
    ShaderResourceCallbackID m_LastUsedCallbackID { INVALID_SHADER_RESOURCE_CALLBACK_ID };
    std::unordered_set<ShaderResourceCallbackID> m_CallbacksToDispatch;
};

enum PipelineStageBitsEnum : uint64
{
    PIPELINE_STAGE_NONE_BIT                    = 0,
    PIPELINE_STAGE_TOP_OF_PIPE_BIT             = BIT(0),
    PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = BIT(1),
    PIPELINE_STAGE_TRANSFER_BIT                = BIT(2),
    PIPELINE_STAGE_VERTEX_SHADER_BIT           = BIT(3),
    PIPELINE_STAGE_FRAGMENT_SHADER_BIT         = BIT(4),
};
using PipelineStageBits = uint64;

enum class TextureLayout : uint16
{
    Undefined = 0,
    PresentSource,
    ColorAttachmentOptimal,
    DepthAttachmentOptimal,
    StencilAttachmentOptimal,
    DepthStencilAttachmentOptimal,
    ShaderReadOnlyOptimal,
    TransferDstOptimal,
    TransferSrcOptimal,
};

enum AccessFlagsBitsEnum : uint64
{
    ACCESS_FLAG_NONE_BIT                           = 0,
    ACCESS_FLAG_VERTEX_ATTRIBUTE_READ_BIT          = BIT(0),
    ACCESS_FLAG_UNIFORM_READ_BIT                   = BIT(1),
    ACCESS_FLAG_INPUT_ATTACHMENT_READ_BIT          = BIT(2),
    ACCESS_FLAG_SHADER_READ_BIT                    = BIT(3),
    ACCESS_FLAG_SHADER_WRITE_BIT                   = BIT(4),
    ACCESS_FLAG_COLOR_ATTACHMENT_READ_BIT          = BIT(5),
    ACCESS_FLAG_COLOR_ATTACHMENT_WRITE_BIT         = BIT(6),
    ACCESS_FLAG_DEPTH_STENCIL_ATTACHMENT_READ_BIT  = BIT(7),
    ACCESS_FLAG_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = BIT(8),
    ACCESS_FLAG_TRANSFER_READ_BIT                  = BIT(9),
    ACCESS_FLAG_TRANSFER_WRITE_BIT                 = BIT(10),
};
using AccessFlagsBits = uint64;

}
