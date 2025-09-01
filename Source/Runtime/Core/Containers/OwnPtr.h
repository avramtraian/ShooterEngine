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
    template<typename Q> friend class OwnPtr;

    template<typename Q>                   friend OwnPtr<Q> AdoptOwn  (Q*);
    template<typename Q, typename... Args> friend OwnPtr<Q> CreateOwn (Args&&...);

    SE_MAKE_NONCOPYABLE(OwnPtr);

public:
    FORCEINLINE OwnPtr()
        : m_Instance(nullptr)
    {}

    FORCEINLINE ~OwnPtr()
    {
        Release();
    }

    FORCEINLINE OwnPtr(OwnPtr&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    FORCEINLINE OwnPtr(NullptrType)
        : m_Instance(nullptr)
    {}

    FORCEINLINE OwnPtr& operator=(OwnPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

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
    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE OwnPtr(OwnPtr<Q>&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE OwnPtr& operator=(OwnPtr<Q>&& other) noexcept
    {
        // Handle the self-assignment case.
        if ((void*)(this) == (void*)(&other))
            return *this;

        Release();

        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

public:
    NODISCARD FORCEINLINE bool IsValid() const
    {
        return m_Instance != nullptr;
    }

    NODISCARD FORCEINLINE T* Get()
    {
        SE_ASSERT(IsValid());
        return m_Instance;
    }

    NODISCARD FORCEINLINE const T* Get() const
    {
        SE_ASSERT(IsValid());
        return m_Instance;
    }

    NODISCARD FORCEINLINE T* GetNonConst() const
    {
        SE_ASSERT(IsValid());
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
            T* instance = m_Instance;
            m_Instance = nullptr;
            delete instance;
        }
    }

    template<typename Q>
    NODISCARD FORCEINLINE OwnPtr<Q> As()
    {
        OwnPtr<Q> casted;
        casted.m_Instance = m_Instance;
        m_Instance = nullptr;
        return casted;
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
    OwnPtr<T> ownPtr;
    ownPtr.m_Instance = new T(Forward<Args>(args)...);
    return ownPtr;
}

}
