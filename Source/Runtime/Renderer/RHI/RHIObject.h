// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/RefPtr.h>

#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace SE
{

// Forward declaration.
class RHIObject;

using RHIObjectCallbackID = uint32;
constexpr RHIObjectCallbackID INVALID_RHI_OBJECT_CALLBACK_ID = 0;
using PFN_RHIObjectCallback = std::function<void(RHIObject&)>;

enum class RHIObjectCallbackType : uint8
{
    Unknown = 0,
    PreDestroy,
};

class RHIObjectCallback
{
    SE_MAKE_NONCOPYABLE(RHIObjectCallback);

public:
    RHIObjectCallback() = default;
    FORCEINLINE ~RHIObjectCallback() { Release(); }

    SHOOTER_API RHIObjectCallback(const RefPtr<RHIObject>& object, RHIObjectCallbackType callbackType, PFN_RHIObjectCallback callback);
    SHOOTER_API RHIObjectCallback(const RefPtr<RHIObject>& object, RHIObjectCallbackType callbackType, RHIObjectCallbackID callbackID);

    SHOOTER_API void Set(const RefPtr<RHIObject>& object, RHIObjectCallbackType callbackType, PFN_RHIObjectCallback callback);
    SHOOTER_API void Set(const RefPtr<RHIObject>& object, RHIObjectCallbackType callbackType, RHIObjectCallbackID callbackID);

    SHOOTER_API void Release();

public:
    FORCEINLINE RHIObjectCallback(RHIObjectCallback&& other) noexcept
        : m_Object(other.m_Object)
        , m_CallackType(other.m_CallackType)
        , m_CallbackID(other.m_CallbackID)
    {
        other.m_Object.Release();
        other.m_CallackType = RHIObjectCallbackType::Unknown;
        other.m_CallbackID = INVALID_RHI_OBJECT_CALLBACK_ID;
    }

    FORCEINLINE RHIObjectCallback& operator=(RHIObjectCallback&& other) noexcept
    {
        // Handle self-assignment case.
        if (this == &other)
            return *this;

        Release();

        m_Object = other.m_Object;
        m_CallackType = other.m_CallackType;
        m_CallbackID = other.m_CallbackID;

        other.m_Object.Release();
        other.m_CallackType = RHIObjectCallbackType::Unknown;
        other.m_CallbackID = INVALID_RHI_OBJECT_CALLBACK_ID;

        return *this;
    }

private:
    WeakRefPtr<RHIObject> m_Object;
    RHIObjectCallbackType m_CallackType { RHIObjectCallbackType::Unknown };
    RHIObjectCallbackID m_CallbackID { INVALID_RHI_OBJECT_CALLBACK_ID };
};

class RHIObject : public RefCounted
{
public:
    RHIObject() = default;
    virtual ~RHIObject() override = default;

public:
    SHOOTER_API RHIObjectCallbackID AddCallbackAndGetID(RHIObjectCallbackType callbackType, PFN_RHIObjectCallback callback);
    SHOOTER_API void RemoveCallbackID(RHIObjectCallbackType callbackType, RHIObjectCallbackID callbackID);

    NODISCARD FORCEINLINE RHIObjectCallback AddCallback(RHIObjectCallbackType callbackType, PFN_RHIObjectCallback callback)
    {
        const RHIObjectCallbackID callbackID = AddCallbackAndGetID(callbackType, std::move(callback));
        RHIObjectCallback scopedCallback;
        scopedCallback.Set(AdoptRef(this), callbackType, callbackID);
        return scopedCallback;
    }

protected:
    SHOOTER_API void DispatchCallbacksOfType(RHIObjectCallbackType callbackType);

private:
    struct Callbacks
    {
        std::unordered_map<RHIObjectCallbackID, PFN_RHIObjectCallback> Functions;
        std::unordered_set<RHIObjectCallbackID> ToDispatch;
        bool IsInDispatch { false };
    };

    RHIObjectCallbackID m_LastUsedCallbackID { INVALID_RHI_OBJECT_CALLBACK_ID };
    std::unordered_map<RHIObjectCallbackType, Callbacks> m_Callbacks;
};

}
