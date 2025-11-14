// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include "RefPtr.h"

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

//========================================================================================================================================//
//----------------------------------------------------- REF COUNTED BASE AND CONTROL -----------------------------------------------------//
//========================================================================================================================================//

class RefCounted
{
    SE_MAKE_NONCOPYABLE(RefCounted);
    SE_MAKE_NONMOVABLE(RefCounted);

public:
    RefCounted()          = default;
    virtual ~RefCounted() = default;
};

class RefCountedControlBlock
{
    SE_MAKE_NONCOPYABLE(RefCountedControlBlock);
    SE_MAKE_NONMOVABLE(RefCountedControlBlock);

public:
    RefCountedControlBlock()  = default;
    ~RefCountedControlBlock() = default;

public:
    NODISCARD ALWAYS_INLINE uint32 GetStrongReferenceCount() const { return m_StrongReferenceCount; }
    NODISCARD ALWAYS_INLINE uint32 GetWeakReferenceCount() const { return m_WeakReferenceCount; }

    ALWAYS_INLINE void IncrementStrongReferenceCount()
    {
        SE_ASSERT(0 < m_StrongReferenceCount && m_StrongReferenceCount < 1000);
        ++m_StrongReferenceCount;
    }

    ALWAYS_INLINE void DecrementStrongReferenceCount()
    {
        SE_ASSERT(0 < m_StrongReferenceCount && m_StrongReferenceCount < 1000);
        --m_StrongReferenceCount;
    }

    ALWAYS_INLINE void IncrementWeakReferenceCount()
    {
        SE_ASSERT(m_WeakReferenceCount < 100);
        ++m_WeakReferenceCount;
    }

    ALWAYS_INLINE void DecrementWeakReferenceCount()
    {
        SE_ASSERT(0 < m_WeakReferenceCount && m_WeakReferenceCount < 100);
        --m_WeakReferenceCount;
    }

private:
    uint32 m_StrongReferenceCount = 1;
    uint32 m_WeakReferenceCount   = 0;
};

// Forward declarations.
template<typename T>
class StrongRefPtr;
template<typename T>
class WeakRefPtr;

template<typename T>
using RefPtr = StrongRefPtr<T>;

//========================================================================================================================================//
//------------------------------------------------------- STRONG REFERENCE POINTER -------------------------------------------------------//
//========================================================================================================================================//

template<typename T>
class StrongRefPtr
{
public:
    template<typename Q>
    friend class StrongRefPtr;

    template<typename Q>
    friend class WeakRefPtr;

    template<typename Q>
    friend StrongRefPtr<Q> AdoptStrongRef(Q*);

    template<typename Q, typename... Args>
    friend StrongRefPtr<Q> CreateStrongRef(Args&&...);

    template<typename Q, typename ConstructorFunction>
    friend StrongRefPtr<Q> CreateStrongRefWithFunction(ConstructorFunction);

public:
    ALWAYS_INLINE StrongRefPtr()
        : m_Instance(nullptr)
    {}

    ALWAYS_INLINE StrongRefPtr(NullptrType)
        : m_Instance(nullptr)
    {}

    ALWAYS_INLINE ~StrongRefPtr()
    {
        // The destructor is a wrapper around the 'Release' API.
        Release();
    }

    ALWAYS_INLINE StrongRefPtr(const StrongRefPtr& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementStrongReferenceCount();
        }
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE StrongRefPtr(const StrongRefPtr<Q>& other)
        : m_Instance(other.m_Instance)
    {
        if (m_Instance)
        {
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementStrongReferenceCount();
        }
    }

    ALWAYS_INLINE StrongRefPtr(StrongRefPtr&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE StrongRefPtr(StrongRefPtr<Q>&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE StrongRefPtr(const WeakRefPtr<Q>& weakRefPtr);

    ALWAYS_INLINE StrongRefPtr& operator=(const StrongRefPtr& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        m_Instance = other.m_Instance;
        if (m_Instance)
        {
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementStrongReferenceCount();
        }

        return *this;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE StrongRefPtr& operator=(const StrongRefPtr<Q>& other)
    {
        // Handle the self-assignment case.
        if (static_cast<const void*>(this) == static_cast<const void*>(&other))
            return *this;

        Release();
        m_Instance = other.m_Instance;
        if (m_Instance)
        {
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementStrongReferenceCount();
        }

        return *this;
    }

    ALWAYS_INLINE StrongRefPtr& operator=(StrongRefPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        m_Instance       = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE StrongRefPtr& operator=(StrongRefPtr<Q>&& other) noexcept
    {
        // Handle the self-assignment case.
        if (static_cast<void*>(this) == static_cast<void*>(&other))
            return *this;

        Release();
        m_Instance       = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE StrongRefPtr& operator=(const WeakRefPtr<Q>& weakRefPtr);

public:
    NODISCARD ALWAYS_INLINE T* Get()
    {
        SE_ASSERT(IsValid());
        return m_Instance;
    }

    NODISCARD ALWAYS_INLINE const T* Get() const
    {
        SE_ASSERT(IsValid());
        return m_Instance;
    }

    NODISCARD ALWAYS_INLINE T* GetNonConst() const
    {
        SE_ASSERT(IsValid());
        return m_Instance;
    }

    NODISCARD ALWAYS_INLINE T* operator->() { return Get(); }
    NODISCARD ALWAYS_INLINE const T* operator->() const { return Get(); }

    NODISCARD ALWAYS_INLINE T& operator*() { return Get(); }
    NODISCARD ALWAYS_INLINE const T& operator*() const { return Get(); }

public:
    NODISCARD ALWAYS_INLINE bool IsValid() const
    {
        // Check if the internal pointer is not null.
        const bool internalPointerIsNull = (m_Instance == nullptr);
        return !internalPointerIsNull;
    }

    ALWAYS_INLINE void Release()
    {
        if (m_Instance)
        {
            // Get the relevant object pointers and invalidate this instance of a reference counted pointer container, since
            // calling the destructor of the held object might invoke the 'Release' function again (re-entrant).
            RefCountedControlBlock* controlBlock = &GetControlBlock();
            RefCounted*             instance     = reinterpret_cast<RefCounted*>(m_Instance);
            m_Instance                           = nullptr;

            if (controlBlock->GetStrongReferenceCount() == 1)
            {
                // We explicitly call the destructor before decrementing the strong reference count as the destructor might release the last weak
                // reference of the same object, which would also free the memory block if both the strong and weak reference counts are zero.
                // By leaving the strong reference count set to 1 when invoking the destructor we ensure that no weak ref pointer will modify the memory block.
                instance->~RefCounted();
                if (controlBlock->GetWeakReferenceCount() == 0)
                {
                    // The object is neither strongly nor weakly referenced anymore.
                    ::operator delete(controlBlock);
                    return;
                }
            }

            // Finally, decrement the strong reference count.
            controlBlock->DecrementStrongReferenceCount();
        }
    }

    template<typename Q>
    NODISCARD ALWAYS_INLINE StrongRefPtr<Q> As() const
    {
        if (m_Instance)
        {
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementStrongReferenceCount();
        }

        Q* castedInstance = static_cast<Q*>(m_Instance);
        return StrongRefPtr<Q>(castedInstance);
    }

public:
    template<typename Q>
    requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD ALWAYS_INLINE bool operator==(const StrongRefPtr<Q>& other) const
    {
        const bool internalPointersAreEqual = (m_Instance == other.m_Instance);
        return internalPointersAreEqual;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD ALWAYS_INLINE bool operator!=(const StrongRefPtr<Q>& other) const
    {
        const bool areEqual = ((*this) == other);
        return !areEqual;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD ALWAYS_INLINE bool operator==(const WeakRefPtr<Q>& weakRefPtr) const;

    template<typename Q>
    requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD ALWAYS_INLINE bool operator!=(const WeakRefPtr<Q>& weakRefPtr) const;

private:
    ALWAYS_INLINE explicit StrongRefPtr(T* instance)
        : m_Instance(instance)
    {}

    NODISCARD ALWAYS_INLINE RefCountedControlBlock& GetControlBlock() const
    {
        void* controlBlockMemoryAddress = reinterpret_cast<uint8*>(m_Instance) - sizeof(RefCountedControlBlock);
        return *static_cast<RefCountedControlBlock*>(controlBlockMemoryAddress);
    }

private:
    T* m_Instance;
};

//========================================================================================================================================//
//-------------------------------------------------------- WEAK REFERENCE POINTER --------------------------------------------------------//
//========================================================================================================================================//

template<typename T>
class WeakRefPtr
{
public:
    template<typename Q>
    friend class StrongRefPtr;

    template<typename Q>
    friend class WeakRefPtr;

    template<typename Q>
    friend WeakRefPtr<Q> AdoptWeakRef(Q*);

public:
    ALWAYS_INLINE WeakRefPtr()
        : m_Instance(nullptr)
    {}

    ALWAYS_INLINE WeakRefPtr(NullptrType)
        : m_Instance(nullptr)
    {}

    ALWAYS_INLINE ~WeakRefPtr()
    {
        // The destructor is a wrapper around the 'Release' API.
        Release();
    }

    ALWAYS_INLINE WeakRefPtr(const WeakRefPtr& other)
        : m_Instance(nullptr)
    {
        if (other.IsValid())
        {
            m_Instance                           = other.m_Instance;
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementWeakReferenceCount();
        }
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE WeakRefPtr(const WeakRefPtr<Q>& other)
        : m_Instance(nullptr)
    {
        if (other.IsValid())
        {
            m_Instance                           = other.m_Instance;
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementWeakReferenceCount();
        }
    }

    ALWAYS_INLINE WeakRefPtr(WeakRefPtr&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE WeakRefPtr(WeakRefPtr<Q>&& other) noexcept
        : m_Instance(other.m_Instance)
    {
        other.m_Instance = nullptr;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE WeakRefPtr(const StrongRefPtr<Q>& strongRefPtr);

    ALWAYS_INLINE WeakRefPtr& operator=(const WeakRefPtr& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        if (other.IsValid())
        {
            m_Instance                           = other.m_Instance;
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementWeakReferenceCount();
        }

        return *this;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE WeakRefPtr& operator=(const WeakRefPtr<Q>& other)
    {
        // Handle the self-assignment case.
        if (static_cast<const void*>(this) == static_cast<const void*>(&other))
            return *this;

        Release();
        if (other.IsValid())
        {
            m_Instance                           = other.m_Instance;
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementWeakReferenceCount();
        }

        return *this;
    }

    ALWAYS_INLINE WeakRefPtr& operator=(WeakRefPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        Release();
        m_Instance       = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE WeakRefPtr& operator=(WeakRefPtr<Q>&& other) noexcept
    {
        // Handle the self-assignment case.
        if (static_cast<void*>(this) == static_cast<void*>(&other))
            return *this;

        Release();
        m_Instance       = other.m_Instance;
        other.m_Instance = nullptr;

        return *this;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    ALWAYS_INLINE WeakRefPtr& operator=(const StrongRefPtr<Q>& strongRefPtr);

public:
    NODISCARD ALWAYS_INLINE T* Get()
    {
        SE_ASSERT(IsValid());
        return m_Instance;
    }

    NODISCARD ALWAYS_INLINE const T* Get() const
    {
        SE_ASSERT(IsValid());
        return m_Instance;
    }

    NODISCARD ALWAYS_INLINE T* GetNonConst() const
    {
        SE_ASSERT(IsValid());
        return m_Instance;
    }

    NODISCARD ALWAYS_INLINE T* operator->() { return Get(); }
    NODISCARD ALWAYS_INLINE const T* operator->() const { return Get(); }

    NODISCARD ALWAYS_INLINE T& operator*() { return Get(); }
    NODISCARD ALWAYS_INLINE const T& operator*() const { return Get(); }

public:
    NODISCARD ALWAYS_INLINE bool IsValid() const
    {
        // Check if the internal pointer is not null.
        const bool internalPointerIsNull = (m_Instance == nullptr);
        if (internalPointerIsNull)
            return false;

        // Check whether the object is strongly referenced.
        const RefCountedControlBlock& controlBlock               = GetControlBlock();
        const bool                    objectIsStronglyReferenced = (controlBlock.GetStrongReferenceCount() > 0);
        return objectIsStronglyReferenced;
    }

    ALWAYS_INLINE void Release()
    {
        if (m_Instance)
        {
            // Decrement the weak reference count.
            RefCountedControlBlock* controlBlock = &GetControlBlock();
            controlBlock->DecrementWeakReferenceCount();

            // The object is neither strongly nor weakly referenced anymore.
            if (controlBlock->GetStrongReferenceCount() == 0 && controlBlock->GetWeakReferenceCount() == 0)
                ::operator delete(controlBlock);

            // Invalidate this weak reference pointer container.
            m_Instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD ALWAYS_INLINE WeakRefPtr<Q> As() const
    {
        Q* castedInstance = static_cast<Q*>(m_Instance);
        return WeakRefPtr<Q>(castedInstance);
    }

public:
    template<typename Q>
    requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD ALWAYS_INLINE bool operator==(const WeakRefPtr<Q>& other) const
    {
        const bool internalPointersAreEqual = (m_Instance == other.m_Instance);
        return internalPointersAreEqual;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD ALWAYS_INLINE bool operator!=(const WeakRefPtr<Q>& other) const
    {
        const bool areEqual = ((*this) == other);
        return !areEqual;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD ALWAYS_INLINE bool operator==(const StrongRefPtr<Q>& strongRefPtr) const;

    template<typename Q>
    requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
    NODISCARD ALWAYS_INLINE bool operator!=(const StrongRefPtr<Q>& strongRefPtr) const;

private:
    ALWAYS_INLINE explicit WeakRefPtr(T* instance)
        : m_Instance(instance)
    {
        if (m_Instance)
        {
            RefCountedControlBlock& controlBlock = GetControlBlock();
            controlBlock.IncrementWeakReferenceCount();
        }
    }

    NODISCARD ALWAYS_INLINE RefCountedControlBlock& GetControlBlock() const
    {
        void* controlBlockMemoryAddress = reinterpret_cast<uint8*>(m_Instance) - sizeof(RefCountedControlBlock);
        return *static_cast<RefCountedControlBlock*>(controlBlockMemoryAddress);
    }

private:
    T* m_Instance;
};

//========================================================================================================================================//
//------------------------------------------------ STRONG REFERENCE POINTER IMPLEMENTATION -----------------------------------------------//
//========================================================================================================================================//

template<typename T>
template<typename Q>
requires (std::is_base_of_v<T, Q>)
StrongRefPtr<T>::StrongRefPtr(const WeakRefPtr<Q>& weakRefPtr)
    : m_Instance(nullptr)
{
    if (weakRefPtr.IsValid())
    {
        m_Instance                           = weakRefPtr.m_Instance;
        RefCountedControlBlock& controlBlock = GetControlBlock();
        controlBlock.IncrementStrongReferenceCount();
    }
}

template<typename T>
template<typename Q>
requires (std::is_base_of_v<T, Q>)
StrongRefPtr<T>& StrongRefPtr<T>::operator=(const WeakRefPtr<Q>& weakRefPtr)
{
    Release();
    if (weakRefPtr.IsValid())
    {
        m_Instance                           = weakRefPtr.m_Instance;
        RefCountedControlBlock& controlBlock = GetControlBlock();
        controlBlock.IncrementStrongReferenceCount();
    }
    return *this;
}

template<typename T>
template<typename Q>
requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
NODISCARD ALWAYS_INLINE bool StrongRefPtr<T>::operator==(const WeakRefPtr<Q>& weakRefPtr) const
{
    const bool internalPointersAreEqual = (m_Instance == weakRefPtr.m_Instance);
    return internalPointersAreEqual;
}

template<typename T>
template<typename Q>
requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
NODISCARD ALWAYS_INLINE bool StrongRefPtr<T>::operator!=(const WeakRefPtr<Q>& weakRefPtr) const
{
    const bool areEqual = ((*this) == weakRefPtr);
    return !areEqual;
}

template<typename T>
NODISCARD ALWAYS_INLINE StrongRefPtr<T> AdoptStrongRef(T* rawInstance)
{
    StrongRefPtr<T> strongRefPtr = StrongRefPtr<T>(rawInstance);
    if (rawInstance)
    {
        RefCountedControlBlock& controlBlock = strongRefPtr.GetControlBlock();
        controlBlock.IncrementStrongReferenceCount();
    }
    return strongRefPtr;
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE StrongRefPtr<T> CreateStrongRef(Args&&... args)
{
    static_assert(sizeof(RefCountedControlBlock) % alignof(T) == 0);
    const usize allocationByteCount = sizeof(RefCountedControlBlock) + sizeof(T);
    uint8*      allocation          = static_cast<uint8*>(::operator new(allocationByteCount));

    new (allocation) RefCountedControlBlock();
    T* instance = new (allocation + sizeof(RefCountedControlBlock)) T(Forward<Args>(args)...);
    return StrongRefPtr<T>(instance);
}

template<typename T, typename ConstructFunction>
NODISCARD ALWAYS_INLINE StrongRefPtr<T> CreateStrongRefWithFunction(ConstructFunction constructFunction)
{
    static_assert(sizeof(RefCountedControlBlock) % alignof(T) == 0);
    const usize allocationByteCount = sizeof(RefCountedControlBlock) + sizeof(T);
    uint8*      allocation          = static_cast<uint8*>(::operator new(allocationByteCount));
    T*          instance            = reinterpret_cast<T*>(allocation + sizeof(RefCountedControlBlock));

    new (allocation) RefCountedControlBlock();
    constructFunction(static_cast<void*>(instance));
    return StrongRefPtr<T>(instance);
}

template<typename T>
NODISCARD ALWAYS_INLINE RefPtr<T> AdoptRef(T* rawInstance)
{
    return AdoptStrongRef<T>(rawInstance);
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE RefPtr<T> CreateRef(Args&&... args)
{
    return CreateStrongRef<T>(Forward<Args>(args)...);
}

template<typename T, typename ConstructFunction>
NODISCARD ALWAYS_INLINE RefPtr<T> CreateRefWithFunction(ConstructFunction constructFunction)
{
    return CreateStrongRefWithFunction<T>(Move(constructFunction));
}

//========================================================================================================================================//
//------------------------------------------------- WEAK REFERENCE POINTER IMPLEMENTATION ------------------------------------------------//
//========================================================================================================================================//

template<typename T>
template<typename Q>
requires (std::is_base_of_v<T, Q>)
WeakRefPtr<T>::WeakRefPtr(const StrongRefPtr<Q>& strongRefPtr)
    : m_Instance(strongRefPtr.m_Instance)
{
    if (m_Instance)
    {
        RefCountedControlBlock& controlBlock = GetControlBlock();
        controlBlock.IncrementWeakReferenceCount();
    }
}

template<typename T>
template<typename Q>
requires (std::is_base_of_v<T, Q>)
WeakRefPtr<T>& WeakRefPtr<T>::operator=(const StrongRefPtr<Q>& strongRefPtr)
{
    Release();
    m_Instance = strongRefPtr.m_Instance;
    if (m_Instance)
    {
        RefCountedControlBlock& controlBlock = GetControlBlock();
        controlBlock.IncrementWeakReferenceCount();
    }
    return *this;
}

template<typename T>
template<typename Q>
requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
NODISCARD ALWAYS_INLINE bool WeakRefPtr<T>::operator==(const StrongRefPtr<Q>& strongRefPtr) const
{
    const bool internalPointersAreEqual = (m_Instance == strongRefPtr.m_Instance);
    return internalPointersAreEqual;
}

template<typename T>
template<typename Q>
requires (std::is_base_of_v<T, Q> || std::is_base_of_v<Q, T>)
NODISCARD ALWAYS_INLINE bool WeakRefPtr<T>::operator!=(const StrongRefPtr<Q>& strongRefPtr) const
{
    const bool areEqual = ((*this) == strongRefPtr);
    return !areEqual;
}

template<typename T>
NODISCARD ALWAYS_INLINE WeakRefPtr<T> AdoptWeakRef(T* rawInstance)
{
    return WeakRefPtr<T>(rawInstance);
}

} // namespace SE
