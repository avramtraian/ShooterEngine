// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

template<typename T>
class OwnPtr
{
public:
    template<typename Q>
    friend class OwnPtr;

    template<typename Q>
    friend OwnPtr<Q> AdoptOwn(Q*);

    SE_MAKE_NONCOPYABLE(OwnPtr);

public:
    FORCEINLINE OwnPtr()
        : m_Instance(nullptr)
    {}

    FORCEINLINE OwnPtr(OwnPtr&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    FORCEINLINE OwnPtr(NullptrType)
        : m_Instance(nullptr)
    {}

    FORCEINLINE ~OwnPtr()
    {
        Release();
    }

    FORCEINLINE OwnPtr& operator=(OwnPtr&& other) noexcept
    {
        if (this == &other)
        {
            /* Handle the self-assignment case. */
            return *this;
        }

        Release();
        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

    FORCEINLINE OwnPtr& operator=(NullptrType)
    {
        Release();
        return *this;
    }

public:
    NODISCARD FORCEINLINE bool IsValid() const { return m_Instance != nullptr; }

    NODISCARD FORCEINLINE T* Get()
    {
        SE_CHECK(IsValid());
        return m_Instance;
    }

    NODISCARD FORCEINLINE const T* Get() const
    {
        SE_CHECK(IsValid());
        return m_Instance;
    }

    NODISCARD FORCEINLINE T* operator->() { return Get(); }
    NODISCARD FORCEINLINE const T* operator->() const { return Get(); }
    
    NODISCARD FORCEINLINE T& operator*() { return *Get(); }
    NODISCARD FORCEINLINE const T& operator*() const { return *Get(); }

public:
    FORCEINLINE void Release()
    {
        if (m_Instance)
        {
            delete m_Instance;
            m_Instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD FORCEINLINE OwnPtr<Q> As()
    {
        Q* instance = static_cast<Q*>(m_Instance);
        m_Instance = nullptr;

        OwnPtr<Q> castedOwnPtr;
        castedOwnPtr.m_Instance = instance;
        return castedOwnPtr;
    }

private:
    T* m_Instance;
};

template<typename T>
NODISCARD FORCEINLINE OwnPtr<T> AdoptOwn(T* instance)
{
    OwnPtr<T> ownPtr;
    ownPtr.m_Instance = instance;
    return ownPtr;
}

template<typename T, typename... Args>
NODISCARD FORCEINLINE OwnPtr<T> CreateOwn(Args&&... args)
{
    T* instance = new T(Forward<Args>(args)...);
    return AdoptOwn<T>(instance);
}

}
