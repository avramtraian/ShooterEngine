// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/RHIObject.h>

namespace SE
{

RHIObjectCallback::RHIObjectCallback(const RefPtr<RHIObject>& object, RHIObjectCallbackType callbackType, PFN_RHIObjectCallback callback)
    : m_Object(object)
{
    if (m_Object.IsValid())
    {
        m_CallackType = callbackType;
        m_CallbackID = m_Object->AddCallbackAndGetID(m_CallackType, std::move(callback));
    }
}

RHIObjectCallback::RHIObjectCallback(const RefPtr<RHIObject>& object, RHIObjectCallbackType callbackType, RHIObjectCallbackID callbackID)
    : m_Object(object)
    , m_CallackType(callbackType)
    , m_CallbackID(callbackID)
{
    SE_ASSERT(m_Object.IsValid());
}

void RHIObjectCallback::Set(const RefPtr<RHIObject>& object, RHIObjectCallbackType callbackType, PFN_RHIObjectCallback callback)
{
    Release();

    m_Object = object;
    if (m_Object.IsValid())
    {
        m_CallackType = callbackType;
        m_CallbackID = m_Object->AddCallbackAndGetID(m_CallackType, std::move(callback));
    }
}

void RHIObjectCallback::Set(const RefPtr<RHIObject>& object, RHIObjectCallbackType callbackType, RHIObjectCallbackID callbackID)
{
    Release();

    m_Object = object;
    m_CallackType = callbackType;
    m_CallbackID = callbackID;
    SE_ASSERT(m_Object.IsValid());
}

void RHIObjectCallback::Release()
{
    if (m_Object.IsValid())
        m_Object->RemoveCallbackID(m_CallackType, m_CallbackID);
    
    m_Object.Release();
    m_CallackType = RHIObjectCallbackType::Unknown;
    m_CallbackID = INVALID_RHI_OBJECT_CALLBACK_ID;
}

RHIObjectCallbackID RHIObject::AddCallbackAndGetID(RHIObjectCallbackType callbackType, PFN_RHIObjectCallback callback)
{
    const RHIObjectCallbackID callbackID = ++m_LastUsedCallbackID;
    SE_ASSERT(callbackType != RHIObjectCallbackType::Unknown);
    m_Callbacks[callbackType].Functions.insert({ callbackID, std::move(callback) });
    return callbackID;
}

void RHIObject::RemoveCallbackID(RHIObjectCallbackType callbackType, RHIObjectCallbackID callbackID)
{
    if (callbackID == INVALID_RHI_OBJECT_CALLBACK_ID)
        return;

    SE_ASSERT(callbackType != RHIObjectCallbackType::Unknown);
    auto& callbacks = m_Callbacks[callbackType];
    SE_ASSERT(callbacks.Functions.contains(callbackID));
    callbacks.Functions.erase(callbackID);
}

void RHIObject::DispatchCallbacksOfType(RHIObjectCallbackType callbackType)
{
    if (callbackType == RHIObjectCallbackType::Unknown)
        return;

    auto& callbacks = m_Callbacks[callbackType];
    if (callbacks.IsInDispatch)
        return;
    callbacks.IsInDispatch = true;

    SE_ASSERT(callbacks.ToDispatch.empty());
    callbacks.ToDispatch.reserve(callbacks.Functions.size());
    for (const auto& [callbackID, function] : callbacks.Functions)
        callbacks.ToDispatch.insert(callbackID);

    for (RHIObjectCallbackID callbackID : callbacks.ToDispatch)
    {
        auto functionIt = callbacks.Functions.find(callbackID);
        if (functionIt == callbacks.Functions.end())
            continue;

        auto& function = (*functionIt).second;
        function(*this);
    }

    callbacks.ToDispatch.clear();
    callbacks.IsInDispatch = false;
}

}
