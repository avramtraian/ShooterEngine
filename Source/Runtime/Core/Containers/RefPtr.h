// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class RefCounted
{
    SE_MAKE_NONCOPYABLE(RefCounted);
    SE_MAKE_NONMOVABLE(RefCounted);

public:
    virtual ~RefCounted() = default;
protected:
    RefCounted() = default;

public:
    FORCEINLINE NODISCARD uint32 GetReferenceCount() const { return m_ReferenceCount; }
    FORCEINLINE NODISCARD bool IsUnreferenced() const { return (m_ReferenceCount == 0); }

    FORCEINLINE void IncrementReferenceCount()
    {
        ++m_ReferenceCount;
    }

    FORCEINLINE void DecrementReferenceCount()
    {
        SE_ASSERT(m_ReferenceCount > 0);
        --m_ReferenceCount;
    }

private:
    uint32 m_ReferenceCount { 1 };
};

template<typename T>
class RefPtr
{
public:
    template<typename Q>
    friend class RefPtr;

    template<typename Q>
    friend RefPtr<Q> AdoptRef(Q*);

    template<typename Q, typename... Args>
    friend RefPtr<Q> CreateRef(Args&&...);

public:
    FORCEINLINE RefPtr()
        : m_Instance(nullptr)
    {}

    FORCEINLINE ~RefPtr()
    {
        Release();
    }
    
    FORCEINLINE RefPtr(const RefPtr& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            IncrementReferenceCount();
        }
    }

    FORCEINLINE RefPtr(RefPtr&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE RefPtr(const RefPtr<Q>& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            IncrementReferenceCount();
        }
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE RefPtr(RefPtr<Q>&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    FORCEINLINE RefPtr(std::nullptr_t)
        : m_Instance(nullptr)
    {}

    FORCEINLINE RefPtr& operator=(const RefPtr& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        m_Instance = other.m_Instance;
        if (m_Instance)
        {
            IncrementReferenceCount();
        }
        return *this;
    }

    FORCEINLINE RefPtr& operator=(RefPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;
        return *this;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE RefPtr& operator=(const RefPtr<Q>& other)
    {
        // Handle the self-assignment case.
        if ((void*)this == (void*)(&other))
            return *this;

        Release();
        m_Instance = other.m_Instance;
        if (m_Instance)
        {
            IncrementReferenceCount();
        }
        return *this;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE RefPtr& operator=(RefPtr<Q>&& other) noexcept
    {
        // Handle the self-assignment case.
        if ((void*)this == (void*)(&other))
            return *this;

        Release();
        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;
        return *this;
    }

    FORCEINLINE RefPtr& operator=(std::nullptr_t)
    {
        Release();
        return *this;
    }

public:
    NODISCARD FORCEINLINE bool IsValid() const { return (m_Instance != nullptr); }
    NODISCARD FORCEINLINE T* Get() { SE_ASSERT(IsValid()); return m_Instance; }
    NODISCARD FORCEINLINE const T* Get() const { SE_ASSERT(IsValid()); return m_Instance; }

    NODISCARD FORCEINLINE T* operator->() { return Get(); }
    NODISCARD FORCEINLINE const T* operator->() const { return Get(); }

    NODISCARD FORCEINLINE T& operator*() { return *Get(); }
    NODISCARD FORCEINLINE const T& operator*() const { return *Get(); }

public:
    FORCEINLINE void Release()
    {
        if (m_Instance)
        {
            DecrementReferenceCount();
            if (IsUnreferenced())
                delete m_Instance;
            m_Instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD FORCEINLINE RefPtr<Q> As() const
    {
        Q* castedInstance = static_cast<Q*>(m_Instance);
        RefPtr<Q> casted = RefPtr<Q>(castedInstance);
        if (casted.m_Instance)
        {
            casted->IncrementReferenceCount();
        }
        return casted;
    }

private:
    FORCEINLINE explicit RefPtr(T* rawInstance)
        : m_Instance(rawInstance)
    {}

    FORCEINLINE void IncrementReferenceCount() { static_cast<RefCounted*>(m_Instance)->IncrementReferenceCount(); }
    FORCEINLINE void DecrementReferenceCount() { static_cast<RefCounted*>(m_Instance)->DecrementReferenceCount(); }
    NODISCARD FORCEINLINE bool IsUnreferenced() const { return static_cast<const RefCounted*>(m_Instance)->IsUnreferenced(); }

private:
    T* m_Instance;
};

template<typename T>
NODISCARD FORCEINLINE RefPtr<T> AdoptRef(T* instance)
{
    RefPtr<T> ref = RefPtr<T>(instance);
    if (ref.m_Instance)
    {
        ref.IncrementReferenceCount();
    }
    return ref;
}

template<typename T>
NODISCARD FORCEINLINE RefPtr<T> AdoptRefWithoutIncrementingReferenceCount(T* instance)
{
    RefPtr<T> ref = RefPtr<T>(instance);
    return ref;
}

template<typename T, typename... Args>
NODISCARD FORCEINLINE RefPtr<T> CreateRef(Args&&... args)
{
    T* instance = new T(Forward<Args>(args)...);
    return RefPtr<T>(instance);
}

}
