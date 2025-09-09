// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/CoreObject/ObjectBase.h>

namespace SE
{

// Internal use only.
SHOOTER_API void InvokeGlobalObjectEnvironment_DestroyObject(ObjectBase* baseObject);

template<typename T>
class SObjectPtr
{
public:
    template<typename Q>
    friend class SObjectPtr;

    template<typename Q>
    friend class WObjectPtr;

    friend class GlobalObjectEnvironment;

public:
    NODISCARD FORCEINLINE static uint64 GetHash(const SObjectPtr& value)
    {
        if (!value.IsValid())
            return 0;
        return (uint64)value.Get();
    }

public:
    FORCEINLINE SObjectPtr()
        : m_Instance(nullptr)
    {}

    FORCEINLINE ~SObjectPtr()
    {
        Release();
    }

    FORCEINLINE SObjectPtr(const SObjectPtr& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            ObjectBase* object = (ObjectBase*)m_Instance;
            object->IncrementReferenceCount();
        }
    }

    FORCEINLINE SObjectPtr(SObjectPtr&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    FORCEINLINE SObjectPtr(std::nullptr_t)
        : m_Instance(nullptr)
    {}

    FORCEINLINE SObjectPtr(T* instance)
        : m_Instance(instance)
    {
        if (m_Instance)
        {
            ObjectBase* object = (ObjectBase*)m_Instance;
            object->IncrementReferenceCount();
        }
    }

    FORCEINLINE SObjectPtr& operator=(const SObjectPtr& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        m_Instance = other.m_Instance;

        if (m_Instance)
        {
            ObjectBase* object = (ObjectBase*)m_Instance;
            object->IncrementReferenceCount();
        }

        return *this;
    }

    FORCEINLINE SObjectPtr& operator=(SObjectPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

    FORCEINLINE SObjectPtr& operator=(std::nullptr_t)
    {
        Release();
        return *this;
    }

public:
    template<typename Q>
    requires (std::is_base_of_v<T, Q> && !std::is_same_v<T, Q>)
    FORCEINLINE SObjectPtr(const SObjectPtr<Q>& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            ObjectBase* object = (ObjectBase*)m_Instance;
            object->IncrementReferenceCount();
        }
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q> && !std::is_same_v<T, Q>)
    FORCEINLINE SObjectPtr(SObjectPtr<Q>&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q> && !std::is_same_v<T, Q>)
    FORCEINLINE SObjectPtr& operator=(const SObjectPtr<Q>& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        m_Instance = other.m_Instance;

        if (m_Instance)
        {
            ObjectBase* object = (ObjectBase*)m_Instance;
            object->IncrementReferenceCount();
        }

        return *this;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q> && !std::is_same_v<T, Q>)
    FORCEINLINE SObjectPtr& operator=(SObjectPtr<Q>&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

public:
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

public:
    NODISCARD FORCEINLINE bool IsValid() const
    {
        // NOTE(Traian): Unlike the weak object pointer, the existance of this strong object pointer
        // instance guarantees that the raw pointer is valid (the object is alive).
        return (m_Instance != nullptr);
    }

    FORCEINLINE void Release()
    {
        if (m_Instance)
        {
            ObjectBase* object = (ObjectBase*)m_Instance;
            m_Instance = nullptr;

            object->DecrementReferenceCount();
            if (object->GetReferenceCount() == 0)
            {
                InvokeGlobalObjectEnvironment_DestroyObject(object);
            }
        }
    }

    template<typename Q>
    NODISCARD FORCEINLINE SObjectPtr<Q> As() const
    {
        if (m_Instance)
        {
            ObjectBase* object = (ObjectBase*)m_Instance;
            object->IncrementReferenceCount();
        }

        SObjectPtr<Q> casted;
        casted.m_Instance = (Q*)m_Instance;
        return casted;
    }

public:
    NODISCARD FORCEINLINE bool operator==(const SObjectPtr& other) const
    {
        return (m_Instance == other.m_Instance);
    }

    NODISCARD FORCEINLINE bool operator!=(const SObjectPtr& other) const
    {
        return (m_Instance != other.m_Instance);
    }

    template<typename Q>
    requires ((std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>) && !std::is_same_v<T, Q>)
    NODISCARD FORCEINLINE bool operator==(const SObjectPtr<Q>& other) const
    {
        return ((void*)(m_Instance) == (void*)(other.m_Instance));
    }

    template<typename Q>
    requires ((std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>) && !std::is_same_v<T, Q>)
    NODISCARD FORCEINLINE bool operator!=(const SObjectPtr<Q>& other) const
    {
        return ((void*)(m_Instance) != (void*)(other.m_Instance));
    }

private:
    T* m_Instance;
};

}
