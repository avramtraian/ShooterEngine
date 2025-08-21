// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/ShaderResource.h>

namespace SE
{

ShaderResourcePreDestroyCallback::ShaderResourcePreDestroyCallback(const RefPtr<ShaderResource>& resource, PFN_ShaderResourcePreDestroyCallback callback)
    : m_Resource(resource)
{
    SE_ASSERT(m_Resource.IsValid());
    m_CallbackID = m_Resource->AddPreDestroyCallbackID(std::move(callback));
}

ShaderResourcePreDestroyCallback::ShaderResourcePreDestroyCallback(const RefPtr<ShaderResource>& resource, ShaderResourceCallbackID callbackID)
    : m_Resource(resource)
    , m_CallbackID(callbackID)
{}

void ShaderResourcePreDestroyCallback::Set(const RefPtr<ShaderResource>& resource, PFN_ShaderResourcePreDestroyCallback callback)
{
    Release();
    m_Resource = resource;
    SE_ASSERT(m_Resource.IsValid());
    m_CallbackID = m_Resource->AddPreDestroyCallbackID(std::move(callback));
}

void ShaderResourcePreDestroyCallback::Set(const RefPtr<ShaderResource>& resource, ShaderResourceCallbackID callbackID)
{
    Release();
    m_Resource = resource;
    m_CallbackID = callbackID;
}

void ShaderResourcePreDestroyCallback::Release()
{
    if (m_Resource.IsValid())
        m_Resource->RemovePreDestroyCallbackID(m_CallbackID);

    m_Resource.Release();
    m_CallbackID = INVALID_SHADER_RESOURCE_CALLBACK_ID;
}

ShaderResourceCallbackID ShaderResource::AddPreDestroyCallbackID(PFN_ShaderResourcePreDestroyCallback callback)
{
    const ShaderResourceCallbackID callbackID = ++m_LastUsedCallbackID;
    m_PreDestroyCallbacks.insert({ callbackID, std::move(callback) });
    return callbackID;
}

void ShaderResource::RemovePreDestroyCallbackID(ShaderResourceCallbackID callbackID)
{
    if (callbackID == INVALID_SHADER_RESOURCE_CALLBACK_ID)
        return;
    
    SE_ASSERT(m_PreDestroyCallbacks.contains(callbackID));
    m_PreDestroyCallbacks.erase(callbackID);
}

void ShaderResource::DispatchPreDestroyCallbacks()
{
    m_CallbacksToDispatch.clear();
    m_CallbacksToDispatch.reserve(m_PreDestroyCallbacks.size());
    for (const auto& [callbackID, callback] : m_PreDestroyCallbacks)
        m_CallbacksToDispatch.insert(callbackID);

    for (ShaderResourceCallbackID callbackID : m_CallbacksToDispatch)
    {
        auto callbackIt = m_PreDestroyCallbacks.find(callbackID);
        if (callbackIt == m_PreDestroyCallbacks.end())
            continue;

        PFN_ShaderResourcePreDestroyCallback& callback = (*callbackIt).second;
        callback(*this);
    }
}

}
