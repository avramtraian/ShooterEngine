// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

// Forward declarations.
template<typename T> class StrongRefPtr;
template<typename T> class WeakRefPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// REF COUNTED CLASS. ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class RefCounted
{
    SE_MAKE_NONCOPYABLE(RefCounted);
    SE_MAKE_NONMOVABLE(RefCounted);
    
    template<typename T> friend class StrongRefPtr;
    template<typename T> friend class WeakRefPtr;

    template<typename Q>                   friend StrongRefPtr<Q> AdoptStrongRef  (Q*);
    template<typename Q, typename... Args> friend StrongRefPtr<Q> CreateStrongRef (Args&&...);
    template<typename Q>                   friend WeakRefPtr<Q>   AdoptWeakRef    (Q*);

public:
    virtual ~RefCounted() = default;

protected:
    FORCEINLINE RefCounted()
        : m_StrongReferenceCount(1)
        , m_WeakReferenceCount(0)
    {}

public:
    NODISCARD FORCEINLINE uint32 GetStrongReferenceCount() const { return m_StrongReferenceCount; }
    NODISCARD FORCEINLINE uint32 GetWeakReferenceCount() const { return m_WeakReferenceCount; }

private:
    FORCEINLINE void IncrementStrongReferenceCount() { ++m_StrongReferenceCount; }
    FORCEINLINE void DecrementStrongReferenceCount() { --m_StrongReferenceCount; }

    FORCEINLINE void IncrementWeakReferenceCount() { ++m_WeakReferenceCount; }
    FORCEINLINE void DecrementWeakReferenceCount() { --m_WeakReferenceCount; }

private:
    uint32 m_StrongReferenceCount;
    uint32 m_WeakReferenceCount;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// STRONG REF PTR CLASS. //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
class StrongRefPtr
{
    template<typename Q>                   friend class StrongRefPtr;
    template<typename Q>                   friend class WeakRefPtr;

    template<typename Q>                   friend StrongRefPtr<Q> AdoptStrongRef                                  (Q*);
    template<typename Q>                   friend StrongRefPtr<Q> AdoptStrongRefWithoutIncrementingReferenceCount (Q*);
    template<typename Q, typename... Args> friend StrongRefPtr<Q> CreateStrongRef                                 (Args&&...);

public:
    FORCEINLINE StrongRefPtr()
        : m_Instance(nullptr)
    {}

    FORCEINLINE ~StrongRefPtr()
    {
        Release();
    }

public:
    FORCEINLINE StrongRefPtr(const StrongRefPtr& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            RefCounted* refCounted = (RefCounted*)m_Instance;
            refCounted->IncrementStrongReferenceCount();
        }
    }

    FORCEINLINE StrongRefPtr(StrongRefPtr&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }
    
    FORCEINLINE StrongRefPtr& operator=(const StrongRefPtr& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();

        m_Instance = other.m_Instance;
        if (m_Instance)
        {
            RefCounted* refCounted = (RefCounted*)m_Instance;
            refCounted->IncrementStrongReferenceCount();
        }

        return *this;
    }

    FORCEINLINE StrongRefPtr& operator=(StrongRefPtr&& other) noexcept
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
    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE StrongRefPtr(const StrongRefPtr<Q>& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            RefCounted* refCounted = (RefCounted*)m_Instance;
            refCounted->IncrementStrongReferenceCount();
        }
    }
    
    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE StrongRefPtr(StrongRefPtr<Q>&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE StrongRefPtr& operator=(const StrongRefPtr<Q>& other)
    {
        // Handle the self-assignment case.
        if ((void*)this == (void*)(&other))
            return *this;

        Release();

        m_Instance = other.m_Instance;
        if (m_Instance)
        {
            RefCounted* refCounted = (RefCounted*)m_Instance;
            refCounted->IncrementStrongReferenceCount();
        }

        return *this;
    }
    
    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE StrongRefPtr& operator=(StrongRefPtr<Q>&& other) noexcept
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
    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    FORCEINLINE StrongRefPtr(const WeakRefPtr<Q>& weakRefPtr);

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    FORCEINLINE StrongRefPtr& operator=(const WeakRefPtr<Q>& weakRefPtr);

    FORCEINLINE StrongRefPtr(std::nullptr_t)
        : m_Instance(nullptr)
    {}

    FORCEINLINE StrongRefPtr& operator=(std::nullptr_t)
    {
        Release();
        return *this;
    }

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    NODISCARD FORCEINLINE bool operator==(const WeakRefPtr<Q>& weakRefPtr) const;

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    NODISCARD FORCEINLINE bool operator!=(const WeakRefPtr<Q>& weakRefPtr) const;

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

    NODISCARD FORCEINLINE T& operator*() { return *Get(); }
    NODISCARD FORCEINLINE const T& operator*() const { return *Get(); }

public:
    NODISCARD FORCEINLINE bool IsValid() const
    {
        return (m_Instance != nullptr);
    }

    FORCEINLINE void Release()
    {
        if (m_Instance)
        {
            RefCounted* refCounted = (RefCounted*)m_Instance;
            refCounted->DecrementStrongReferenceCount();

            if (refCounted->GetStrongReferenceCount() == 0)
            {
                const uint32 weakReferenceCount = refCounted->GetWeakReferenceCount();
                m_Instance->~T();

                if (weakReferenceCount > 0)
                {
                    new (refCounted) RefCounted();
                    refCounted->m_StrongReferenceCount = 0;
                    refCounted->m_WeakReferenceCount = weakReferenceCount;
                }
                else
                {
                    ::operator delete(m_Instance);
                }
            }

            m_Instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD FORCEINLINE StrongRefPtr<Q> As() const
    {
        StrongRefPtr<Q> casted;

        if (m_Instance)
        {
            RefCounted* refCounted = (RefCounted*)m_Instance;
            refCounted->IncrementStrongReferenceCount();
            casted.m_Instance = static_cast<Q*>(m_Instance);
        }

        return casted;
    }

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    NODISCARD FORCEINLINE bool operator==(const StrongRefPtr<Q>& other) const
    {
        return (m_Instance == other.m_Instance);
    }

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    NODISCARD FORCEINLINE bool operator!=(const StrongRefPtr<Q>& other) const
    {
        const bool areEqual = ((*this) == other);
        return !areEqual;
    }

private:
    T* m_Instance;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// WEAK REF PTR CLASS. ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
class WeakRefPtr
{
    template<typename Q> friend class StrongRefPtr;
    template<typename Q> friend class WeakRefPtr;

    template<typename Q> friend WeakRefPtr<Q> AdoptWeakRef (Q*);

public:
    FORCEINLINE WeakRefPtr()
        : m_Instance(nullptr)
    {}

    FORCEINLINE ~WeakRefPtr()
    {
        Release();
    }

public:
    FORCEINLINE WeakRefPtr(const WeakRefPtr& other)
        : m_Instance(nullptr)
    {
        if (other.IsValid())
        {
            RefCounted* refCounted = (RefCounted*)other.m_Instance;
            refCounted->IncrementWeakReferenceCount();
            m_Instance = other.m_Instance;
        }
    }

    FORCEINLINE WeakRefPtr(WeakRefPtr&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    FORCEINLINE WeakRefPtr& operator=(const WeakRefPtr& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();

        if (other.IsValid())
        {
            RefCounted* refCounted = (RefCounted*)other.m_Instance;
            refCounted->IncrementWeakReferenceCount();
            m_Instance = other.m_Instance;
        }

        return *this;
    }

    FORCEINLINE WeakRefPtr& operator=(WeakRefPtr&& other) noexcept
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
    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE WeakRefPtr(const WeakRefPtr<Q>& other)
        : m_Instance(nullptr)
    {
        if (other.IsValid())
        {
            RefCounted* refCounted = (RefCounted*)other.m_Instance;
            refCounted->IncrementWeakReferenceCount();
            m_Instance = other.m_Instance;
        }
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE WeakRefPtr(WeakRefPtr<Q>&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE WeakRefPtr& operator=(const WeakRefPtr<Q>& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();

        if (other.IsValid())
        {
            RefCounted* refCounted = (RefCounted*)other.m_Instance;
            refCounted->IncrementWeakReferenceCount();
            m_Instance = other.m_Instance;
        }

        return *this;
    }

    template<typename Q>
    requires(!std::is_same_v<T, Q> && std::is_base_of_v<T, Q>)
    FORCEINLINE WeakRefPtr& operator=(WeakRefPtr<Q>&& other) noexcept
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
    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    FORCEINLINE WeakRefPtr(const StrongRefPtr<Q>& strongRefPtr);

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    FORCEINLINE WeakRefPtr& operator=(const StrongRefPtr<Q>& strongRefPtr);

    FORCEINLINE WeakRefPtr(std::nullptr_t)
        : m_Instance(nullptr)
    {}

    FORCEINLINE WeakRefPtr& operator=(std::nullptr_t)
    {
        Release();
        return *this;
    }

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    NODISCARD FORCEINLINE bool operator==(const StrongRefPtr<Q>& strongRefPtr) const;

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    NODISCARD FORCEINLINE bool operator!=(const StrongRefPtr<Q>& strongRefPtr) const;

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

    NODISCARD FORCEINLINE T& operator*() { return *Get(); }
    NODISCARD FORCEINLINE const T& operator*() const { return *Get(); }

public:
    NODISCARD FORCEINLINE bool IsValid() const
    {
        if (m_Instance == nullptr)
            return false;

        const RefCounted* refCounted = (const RefCounted*)m_Instance;
        return (refCounted->GetStrongReferenceCount() > 0);
    }

    FORCEINLINE void Release()
    {
        if (m_Instance)
        {
            RefCounted* refCounted = (RefCounted*)m_Instance;
            refCounted->DecrementWeakReferenceCount();
            if (refCounted->GetStrongReferenceCount() == 0 && refCounted->GetWeakReferenceCount() == 0)
            {
                // NOTE(Traian): Since the strong reference count is zero, we know for sure that 'm_Instance' is actually a pointer to
                // a plain 'RefCounted' object (not a derived class) and thus no destructor is required.

                // NOTE(Traian): Another reason to not call the 'RefCounted' destructor is the following scenario: let's imagine we have
                // one strong reference pointer and one weak reference pointer. When the strong pointer is released, the destructor of the
                // instance (for whatever reason) also releases the weak pointer. This scenario would lead us exactly to this codepath, but
                // the strong pointer doesn't yet replace the object stored at the 'm_Instance' address with a plain 'RefCounted' object.
                // Because the 'RefCounted' destructor is marked as virtual, calling 'm_Instance->~RefCounted()' would invoke the object
                // destructor AGAIN. So the bug would be that the same destructor is called twice!

                ::operator delete(m_Instance);
            }

            m_Instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD FORCEINLINE WeakRefPtr<Q> As() const
    {
        WeakRefPtr<Q> casted;
        if (m_Instance)
        {
            RefCounted* refCounted = (RefCounted*)m_Instance;
            refCounted->IncrementWeakReferenceCount();
            casted.m_Instance = static_cast<Q*>(m_Instance);
        }
        return casted;
    }

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    NODISCARD FORCEINLINE bool operator==(const WeakRefPtr<Q>& other) const
    {
        const bool thisIsValid = IsValid();
        const bool otherIsValid = other.IsValid();
        
        // One pointer is valid and the other is not valid, so they can't be the same.
        if (thisIsValid != otherIsValid)
            return false;

        // Both pointers are not valid, and thus they are the same.
        if (!thisIsValid)
            return true;

        return (m_Instance == other.m_Instance);
    }

    template<typename Q>
    requires(std::is_base_of_v<T, Q>)
    NODISCARD FORCEINLINE bool operator!=(const WeakRefPtr<Q>& other) const
    {
        const bool areEqual = ((*this) == other);
        return !areEqual;
    }

private:
    T* m_Instance;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// STRONG REF PTR METHOD IMPLEMENTATIONS. /////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
template<typename Q>
requires(std::is_base_of_v<T, Q>)
StrongRefPtr<T>::StrongRefPtr(const WeakRefPtr<Q>& weakRefPtr)
    : m_Instance(nullptr)
{
    if (weakRefPtr.IsValid())
    {
        m_Instance = weakRefPtr.m_Instance;
        RefCounted* refCounted = (RefCounted*)m_Instance;
        refCounted->IncrementStrongReferenceCount();
    }
}

template<typename T>
template<typename Q>
requires(std::is_base_of_v<T, Q>)
StrongRefPtr<T>& StrongRefPtr<T>::operator=(const WeakRefPtr<Q>& weakRefPtr)
{
    // Avoid redundant strong reference count decrement and increment.
    if (m_Instance == weakRefPtr.m_Instance)
        return *this;

    Release();
    if (weakRefPtr.IsValid())
    {
        m_Instance = weakRefPtr.m_Instance;
        RefCounted* refCounted = (RefCounted*)m_Instance;
        refCounted->IncrementStrongReferenceCount();
    }

    return *this;
}

template<typename T>
template<typename Q>
requires(std::is_base_of_v<T, Q>)
bool StrongRefPtr<T>::operator==(const WeakRefPtr<Q>& weakRefPtr) const
{
    // NOTE(Traian): Just comparing raw pointers is not enough, because a weak reference pointer can be invalid even
    // when its internal instance pointer is not null (when the instance _strong_ reference count is zero while the
    // _weak_ reference count is not), and thus the following check is required to ensure correct behaviour.
    if (!IsValid() && !weakRefPtr.IsValid())
        return true;

    return (m_Instance == weakRefPtr.m_Instance);
}

template<typename T>
template<typename Q>
requires(std::is_base_of_v<T, Q>)
bool StrongRefPtr<T>::operator!=(const WeakRefPtr<Q>& weakRefPtr) const
{
    const bool areEqual = ((*this) == weakRefPtr);
    return !areEqual;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// WEAK REF PTR METHOD IMPLEMENTATIONS. //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
template<typename Q>
requires(std::is_base_of_v<T, Q>)
WeakRefPtr<T>::WeakRefPtr(const StrongRefPtr<Q>& strongRefPtr)
    : m_Instance(strongRefPtr.m_Instance)
{
    if (m_Instance)
    {
        RefCounted* refCounted = (RefCounted*)m_Instance;
        refCounted->IncrementWeakReferenceCount();
    }
}

template<typename T>
template<typename Q>
requires(std::is_base_of_v<T, Q>)
WeakRefPtr<T>& WeakRefPtr<T>::operator=(const StrongRefPtr<Q>& strongRefPtr)
{
    // Avoid redundant weak reference count decrement and increment.
    if (m_Instance == strongRefPtr.m_Instance)
        return *this;

    Release();

    m_Instance = strongRefPtr.m_Instance;
    if (m_Instance)
    {
        RefCounted* refCounted = (RefCounted*)m_Instance;
        refCounted->IncrementWeakReferenceCount();
    }

    return *this;
}

template<typename T>
template<typename Q>
requires(std::is_base_of_v<T, Q>)
bool WeakRefPtr<T>::operator==(const StrongRefPtr<Q>& strongRefPtr) const
{
    // NOTE(Traian): Just comparing raw pointers is not enough, because a weak reference pointer can be invalid even
    // when its internal instance pointer is not null (when the instance _strong_ reference count is zero while the
    // _weak_ reference count is not), and thus the following check is required to ensure correct behaviour.
    if (!IsValid() && !strongRefPtr.IsValid())
        return true;

    return (m_Instance == strongRefPtr.m_Instance);
}

template<typename T>
template<typename Q>
requires(std::is_base_of_v<T, Q>)
bool WeakRefPtr<T>::operator!=(const StrongRefPtr<Q>& strongRefPtr) const
{
    const bool areEqual = ((*this) == strongRefPtr);
    return !areEqual;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// CREATION AND ADOPTION. //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
NODISCARD FORCEINLINE StrongRefPtr<T> AdoptStrongRef(T* instance)
{
    StrongRefPtr<T> strongRefPtr;
    if (instance)
    {
        strongRefPtr.m_Instance = instance;
        RefCounted* refCounted = (RefCounted*)instance;
        refCounted->IncrementStrongReferenceCount();
    }
    return strongRefPtr;
}

template<typename T>
NODISCARD FORCEINLINE StrongRefPtr<T> AdoptStrongRefWithoutIncrementingReferenceCount(T* instance)
{
    StrongRefPtr<T> strongRefPtr;
    strongRefPtr.m_Instance = instance;
    return strongRefPtr;
}

template<typename T, typename... Args>
NODISCARD FORCEINLINE StrongRefPtr<T> CreateStrongRef(Args&&... args)
{
    // NOTE(Traian): The default constructor of the RefCounted class initializes the strong reference count
    // to 1, so no increment is required here. The reason behind this decision is that the pointer can be adopted
    // during the object constructor. When the adoption happens, the reference count would be incresed from zero to
    // one, and if the strong ref ptr that adopted it is also destroyed during the constructor, the reference count
    // would go from one to zero, and thus the object will be destroyed!

    StrongRefPtr<T> strongRefPtr;
    strongRefPtr.m_Instance = new T(Forward<Args>(args)...);
    return strongRefPtr;
}

template<typename T>
NODISCARD FORCEINLINE WeakRefPtr<T> AdoptWeakRef(T* instance)
{
    WeakRefPtr<T> weakRefPtr;
    if (instance)
    {
        weakRefPtr.m_Instance = instance;
        RefCounted* refCounted = (RefCounted*)instance;
        SE_ENSURE(refCounted->GetStrongReferenceCount() > 0);
        refCounted->IncrementWeakReferenceCount();
    }
    return weakRefPtr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// LEGACY POINTER NAMES. //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
using RefPtr = StrongRefPtr<T>;

template<typename T>
NODISCARD FORCEINLINE RefPtr<T> AdoptRef(T* instance)
{
    return AdoptStrongRef<T>(instance);
}

template<typename T, typename... Args>
NODISCARD FORCEINLINE RefPtr<T> CreateRef(Args&&... args)
{
    return CreateStrongRef<T>(Forward<Args>(args)...);
}

}
