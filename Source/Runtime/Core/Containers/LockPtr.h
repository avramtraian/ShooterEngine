// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class LockCounted
{
    SE_MAKE_NONCOPYABLE(LockCounted);
    SE_MAKE_NONMOVABLE(LockCounted);

public:
    LockCounted() = default;
    ~LockCounted() = default;

    NODISCARD FORCEINLINE uint32 GetLockCount() const { return m_LockCount; }
    NODISCARD FORCEINLINE bool IsLocked() const { return (m_LockCount > 0); }
    NODISCARD FORCEINLINE bool IsUnlocked() const { return (m_LockCount == 0); }

    FORCEINLINE void IncrementLockCount()
    {
        if (IsUnlocked())
            OnLock();
        ++m_LockCount;
    }

    FORCEINLINE void DecrementLockCount()
    {
        SE_ASSERT(IsLocked());
        --m_LockCount;
        if (IsUnlocked())
            OnUnlock();
    }

protected:
    FORCEINLINE virtual void OnLock() {}
    FORCEINLINE virtual void OnUnlock() {}

private:
    uint32 m_LockCount { 0 };
};

template<typename T>
class LockPtr
{
public:
    FORCEINLINE LockPtr()
        : m_Instance(nullptr)
    {}

    FORCEINLINE ~LockPtr()
    {
        Release();
    }

    FORCEINLINE LockPtr(const LockPtr& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            LockCounted* lockCounted = (LockCounted*)m_Instance;
            lockCounted->IncrementLockCount();
        }
    }

    FORCEINLINE LockPtr(LockPtr&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    FORCEINLINE LockPtr(T* instance)
        : m_Instance(instance)
    {
        if (m_Instance)
        {
            LockCounted* lockCounted = (LockCounted*)m_Instance;
            lockCounted->IncrementLockCount();
        }
    }

    FORCEINLINE LockPtr(std::nullptr_t)
        : m_Instance(nullptr)
    {}

    FORCEINLINE LockPtr& operator=(const LockPtr& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();

        m_Instance = other.m_Instance;
        if (m_Instance)
        {
            LockCounted* lockCounted = (LockCounted*)m_Instance;
            lockCounted->IncrementLockCount();
        }

        return *this;
    }

    FORCEINLINE LockPtr& operator=(LockPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();

        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

    FORCEINLINE LockPtr& operator=(T* instance)
    {
        // Avoid a redundant lock count decrement and increment.
        if (m_Instance == instance)
            return *this;

        Release();

        m_Instance = instance;
        if (m_Instance)
        {
            LockCounted* lockCounted = (LockCounted*)m_Instance;
            lockCounted->IncrementLockCount();
        }

        return *this;
    }

    FORCEINLINE LockPtr& operator=(std::nullptr_t)
    {
        Release();
        return *this;
    }

public:
    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE LockPtr(const LockPtr<Q>& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            LockCounted* lockCounted = (LockCounted*)m_Instance;
            lockCounted->IncrementLockCount();
        }
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE LockPtr(LockPtr<Q>&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE LockPtr(Q* instance)
        : m_Instance(instance)
    {
        if (m_Instance)
        {
            LockCounted* lockCounted = (LockCounted*)m_Instance;
            lockCounted->IncrementLockCount();
        }
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE LockPtr& operator=(const LockPtr<Q>& other)
    {
        // Handle the self-assignment case.
        if ((void*)(this) == (void*)(&other))
            return *this;

        Release();

        m_Instance = other.m_Instance;
        if (m_Instance)
        {
            LockCounted* lockCounted = (LockCounted*)m_Instance;
            lockCounted->IncrementLockCount();
        }

        return *this;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE LockPtr& operator=(LockPtr<Q>&& other) noexcept
    {
        // Handle the self-assignment case.
        if ((void*)(this) == (void*)(&other))
            return *this;

        Release();

        m_Instance = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE LockPtr& operator=(Q* instance)
    {
        // Avoid a redundant lock count decrement and increment.
        if (m_Instance == instance)
            return *this;

        Release();

        m_Instance = instance;
        if (m_Instance)
        {
            LockCounted* lockCounted = (LockCounted*)m_Instance;
            lockCounted->IncrementLockCount();
        }

        return *this;
    }

public:
    NODISCARD FORCEINLINE bool IsValid() const { return (m_Instance != nullptr); }

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
            LockCounted* lockCounted = (LockCounted*)m_Instance;
            lockCounted->DecrementLockCount();
            m_Instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD FORCEINLINE LockPtr<Q> As() const
    {
        LockPtr<Q> casted = static_cast<Q*>(m_Instance);
        return casted;
    }

    template<typename Q>
    requires(std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD FORCEINLINE bool operator==(const LockPtr<Q>& other) const
    {
        return (m_Instance == other.m_Instance);
    }

    template<typename Q>
    requires(std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD FORCEINLINE bool operator!=(const LockPtr<Q>& other) const
    {
        return (m_Instance != other.m_Instance);
    }

private:
    T* m_Instance;
};

}
