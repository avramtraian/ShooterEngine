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
    friend OwnPtr<Q> adopt_own(Q*);

    SE_MAKE_NONCOPYABLE(OwnPtr);

public:
    FORCEINLINE OwnPtr()
        : m_instance(nullptr)
    {}

    FORCEINLINE OwnPtr(OwnPtr&& other) noexcept
        : m_instance(other.m_instance)
    {
        other.m_instance = nullptr;
    }

    FORCEINLINE OwnPtr(NullptrType)
        : m_instance(nullptr)
    {}

    FORCEINLINE ~OwnPtr()
    {
        release();
    }

    FORCEINLINE OwnPtr& operator=(OwnPtr&& other) noexcept
    {
        if (this == &other)
        {
            /* Handle the self-assignment case. */
            return *this;
        }

        release();
        m_instance = other.m_instance;
        other.m_instance = nullptr;

        return *this;
    }

    FORCEINLINE OwnPtr& operator=(NullptrType)
    {
        release();
        return *this;
    }

public:
    NODISCARD FORCEINLINE bool is_valid() const { return m_instance != nullptr; }

    NODISCARD FORCEINLINE T* get()
    {
        SE_CHECK(is_valid());
        return m_instance;
    }

    NODISCARD FORCEINLINE const T* get() const
    {
        SE_CHECK(is_valid());
        return m_instance;
    }

    NODISCARD FORCEINLINE T* operator->() { return get(); }
    NODISCARD FORCEINLINE const T* operator->() const { return get(); }
    
    NODISCARD FORCEINLINE T& operator*() { return *get(); }
    NODISCARD FORCEINLINE const T& operator*() const { return *get(); }

public:
    FORCEINLINE void release()
    {
        if (m_instance)
        {
            delete m_instance;
            m_instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD FORCEINLINE OwnPtr<Q> as()
    {
        Q* instance = static_cast<Q*>(m_instance);
        m_instance = nullptr;

        OwnPtr<Q> own_ptr;
        own_ptr.m_instance = instance;
        return own_ptr;
    }

private:
    T* m_instance;
};

template<typename T>
NODISCARD FORCEINLINE OwnPtr<T> adopt_own(T* instance)
{
    OwnPtr<T> own_ptr;
    own_ptr.m_instance = instance;
    return own_ptr;
}

template<typename T, typename... Args>
NODISCARD FORCEINLINE OwnPtr<T> create_own(Args&&... args)
{
    T* instance = new T(forward<Args>(args)...);
    return adopt_own<T>(instance);
}

}
