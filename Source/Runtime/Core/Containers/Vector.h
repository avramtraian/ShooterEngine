// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/Allocator.h>
#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/Core/Containers/VectorView.h>
#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

//
// Certain operations on the Vector container may invoke element constructors or destructors,
// which in turn could call functions on the same Vector. For example, calling 'Remove'
// might trigger the destructor of an element, which could then call 'Add' on the same Vector!
// Since the internal state of the Vector may be inconsistent during such operations, this could
// lead to undefined behavior.
//
// To help prevent these issues, a protection mechanism is implemented that triggers an assert
// if such a reentrant call is detected. This mechanism can be enabled by setting the macro
// 'SE_VECTOR_CHECK_PROTECTION' to 1.
//
#define SE_VECTOR_CHECK_PROTECTION 1

//
// Dynamic linear array of typed elements. Assumes that elements implement a move constructor, since
// re-locating elements from one buffer to a another buffer is critical. Since the internal memory
// buffer may change when adding/removing elements, references and pointers to the elements stored
// in the container are guaranteed to remain valid only when no additions or removals happen.
// 
// The Vector container is not reentrant. It doesn't implement any features to support concurrent access
// to it, and even when used exclusively by a single thread, no reentrant function calls are allowed (the
// constructor/destructor of a element stored in the container is not allowed to call any other function
// on the same container) - see the 'SE_VECTOR_CHECK_PROTECTION' macro.
//
template<typename ElementType, typename Allocator = DefaultAllocator>
class Vector
{
public:
    template<typename FriendElementType, typename FriendAllocator>
    friend class Vector;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    static constexpr usize GROWTH_FACTOR_NUMERATOR = 3;
    static constexpr usize GROWTH_FACTOR_DENOMINATOR = 2;
    static_assert(GROWTH_FACTOR_NUMERATOR > GROWTH_FACTOR_DENOMINATOR);
    static_assert(GROWTH_FACTOR_DENOMINATOR != 0);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
#if SE_VECTOR_CHECK_PROTECTION
    class ScopedProtectionLock
    {
        SE_MAKE_NONCOPYABLE(ScopedProtectionLock);
        SE_MAKE_NONMOVABLE(ScopedProtectionLock);

    public:
        FORCEINLINE ScopedProtectionLock(const Vector<ElementType, Allocator>& vector)
            : m_Vector(vector)
        {
            SE_ENSURE(m_Vector.IsNotProtected());
            m_Vector.SetIsProtected(true);
        }

        FORCEINLINE ~ScopedProtectionLock()
        {
            SE_ENSURE(m_Vector.IsProtected());
            m_Vector.SetIsProtected(false);
        }

    private:
        const Vector<ElementType, Allocator>& m_Vector;
    };
#endif // SE_VECTOR_CHECK_PROTECTION

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SE_VECTOR_CHECK_PROTECTION
    template<typename OtherAllocator>
    using OtherScopedProtectionLock = Vector<ElementType, OtherAllocator>::ScopedProtectionLock;
#endif // SE_VECTOR_CHECK_PROTECTION

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE Vector()
        : m_Elements(nullptr)
        , m_Capacity(0)
        , m_Count(0)
        , m_IsProtected(0)
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE ~Vector()
    {
        ClearAndShrink();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename OtherAllocator>
    FORCEINLINE Vector(const Vector<ElementType, OtherAllocator>& other)
        : m_Elements(nullptr)
        , m_Capacity(0)
        , m_Count(0)
        , m_IsProtected(0)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(other.IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        if (other.m_Count > 0)
        {
#if SE_VECTOR_CHECK_PROTECTION
            OtherScopedProtectionLock<OtherAllocator> otherProtectionLock(other);
            ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

            ReAllocateElementsBuffer(other.m_Count);
            m_Count = other.m_Count;
            Vector::CopyElements(m_Elements, other.m_Elements, m_Count);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE Vector(const Vector& other)
        : m_Elements(nullptr)
        , m_Capacity(0)
        , m_Count(0)
        , m_IsProtected(0)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(other.IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        if (other.m_Count > 0)
        {
#if SE_VECTOR_CHECK_PROTECTION
            OtherScopedProtectionLock<Allocator> otherProtectionLock(other);
            ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

            ReAllocateElementsBuffer(other.m_Count);
            m_Count = other.m_Count;
            Vector::CopyElements(m_Elements, other.m_Elements, m_Count);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE Vector(Vector<ElementType, Allocator>&& other) noexcept
        : m_Elements(other.m_Elements)
        , m_Capacity(other.m_Capacity)
        , m_Count(other.m_Count)
        , m_IsProtected(0)
    {
#if SE_VECTOR_CHECK_PROTECTION
        // NOTE(Traian): We don't need to acquire any protection locks because no elements are actually
        // instantiated or destroyed, and thus no external constructors/destructors are called.
        SE_ENSURE(other.IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        other.m_Elements = nullptr;
        other.m_Capacity = 0;
        other.m_Count = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE Vector(std::initializer_list<ElementType> initiatlizerList)
        : m_Elements(nullptr)
        , m_Capacity(0)
        , m_Count(0)
        , m_IsProtected(0)
    {
        if (initiatlizerList.size() > 0)
        {
#if SE_VECTOR_CHECK_PROTECTION
            ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

            ReAllocateElementsBuffer(initiatlizerList.size());
            m_Count = initiatlizerList.size();
            Vector::CopyElements(m_Elements, initiatlizerList.begin(), m_Count);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename OtherAllocator>
    FORCEINLINE Vector& operator=(const Vector<ElementType, OtherAllocator>& other)
    {
        // Handle the self-assignment case.
        if ((void*)this == (void*)(&other))
            return *this;

#if SE_VECTOR_CHECK_PROTECTION
        // NOTE(Traian): We only check if the other vector is not locked and not this vector because the
        // 'Clear' function does the same thing (and also acquires the protection lock).
        SE_ENSURE(other.IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        Clear();

        if (other.m_Count > 0)
        {
#if SE_VECTOR_CHECK_PROTECTION
            OtherScopedProtectionLock<OtherAllocator> otherProtectionLock(other);
            ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

            if (m_Capacity < other.m_Count)
            {
                ReAllocateElementsBuffer(other.m_Count);
            }

            m_Count = other.m_Count;
            Vector::CopyElements(m_Elements, other.m_Elements, m_Count);
        }

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE Vector& operator=(const Vector& other)
    {
        // Forward the implementation to the templated version.
        return this->operator=<Allocator>(other);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE Vector& operator=(Vector<ElementType, Allocator>&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

#if SE_VECTOR_CHECK_PROTECTION
        // NOTE(Traian): We only check if the other vector is not locked and not this vector because the
        // 'Clear' function does the same thing (and also acquires the protection lock).
        SE_ENSURE(other.IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        Clear();

        // NOTE(Traian): We don't need to acquire any protection locks because no elements are actually
        // instantiated or destroyed, and thus no external constructors/destructors are called.

        m_Elements = other.m_Elements;
        m_Capacity = other.m_Capacity;
        m_Count = other.m_Count;

        other.m_Elements = nullptr;
        other.m_Capacity = 0;
        other.m_Count = 0;

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE Vector& operator=(std::initializer_list<ElementType> initiatlizerList)
    {
        Clear();

        if (initiatlizerList.size() > 0)
        {
#if SE_VECTOR_CHECK_PROTECTION
            ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

            ReAllocateElementsBuffer(initiatlizerList.size());
            m_Count = initiatlizerList.size();
            Vector::CopyElements(m_Elements, initiatlizerList.begin(), m_Count);
        }

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD FORCEINLINE ElementType* Elements()
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        return m_Elements;
    }

    NODISCARD FORCEINLINE const ElementType* Elements() const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        return m_Elements;
    }

    NODISCARD FORCEINLINE ElementType* NonConstElements() const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        return m_Elements;
    }

    NODISCARD FORCEINLINE usize Capacity() const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION
        
        return m_Capacity;
    }

    NODISCARD FORCEINLINE usize Count() const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        return m_Count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE bool IsEmpty() const { return (Count() == 0); }
    NODISCARD FORCEINLINE bool HasElements() const { return (Count() > 0); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE VectorView<ElementType> View() { return VectorView<ElementType>(Elements(), Count()); }
    NODISCARD FORCEINLINE VectorView<const ElementType> View() const { return VectorView<const ElementType>(Elements(), Count()); }

    NODISCARD FORCEINLINE VectorView<const ElementType> ConstView() const { return VectorView<const ElementType>(Elements(), Count()); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD FORCEINLINE ElementType& At(usize index)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(index < m_Count);
        return m_Elements[index];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE const ElementType& At(usize index) const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(index < m_Count);
        return m_Elements[index];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE ElementType& operator[](usize index) { return At(index); }
    NODISCARD FORCEINLINE const ElementType& operator[](usize index) const { return At(index); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE ElementType& First()
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(HasElements());
        return m_Elements[0];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE const ElementType& First() const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(HasElements());
        return m_Elements[0];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE ElementType& Last()
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(HasElements());
        return m_Elements[m_Count - 1];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE const ElementType& Last() const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(HasElements());
        return m_Elements[m_Count - 1];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE void Add(const ElementType& element)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        if (m_Capacity < m_Count + 1)
        {
            const usize newCapacity = CalculateNextCapacity(m_Capacity, m_Count + 1);
            ReAllocateElementsBuffer(newCapacity);
        }

        new (m_Elements + m_Count) ElementType(element);
        ++m_Count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void Add(ElementType&& element)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        if (m_Capacity < m_Count + 1)
        {
            const usize newCapacity = CalculateNextCapacity(m_Capacity, m_Count + 1);
            ReAllocateElementsBuffer(newCapacity);
        }

        new (m_Elements + m_Count) ElementType(Move(element));
        ++m_Count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void Add(std::initializer_list<ElementType> initializerList)
    {
        EnsureCapacity(m_Count + initializerList.size());

        for (usize index = 0; index < initializerList.size(); ++index)
            Add(initializerList.begin()[index]);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename... Args>
    FORCEINLINE ElementType& Emplace(Args&&... args)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        if (m_Capacity < m_Count + 1)
        {
            const usize newCapacity = CalculateNextCapacity(m_Capacity, m_Count + 1);
            ReAllocateElementsBuffer(newCapacity);
        }

        new (m_Elements + m_Count) ElementType(Forward<Args>(args)...);
        return m_Elements[m_Count++];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE void PopBack(usize popCount = 1)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(popCount <= m_Count);
        for (usize elementIndex = m_Count - popCount; elementIndex < m_Count; ++elementIndex)
            m_Elements[elementIndex].~ElementType();
        m_Count -= popCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void RemoveIndex(usize elementIndex)
    {
        RemoveIndices(elementIndex, 1);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void RemoveIndices(usize startElementIndex, usize removeCount)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(startElementIndex + removeCount <= m_Count);

        for (usize offset = 0; offset < removeCount; ++offset)
            m_Elements[startElementIndex + offset].~ElementType();

        const usize remainingCount = m_Count - (startElementIndex + removeCount);
        const usize moveStartIndex = startElementIndex + removeCount;

        for (usize offset = 0; offset < remainingCount; ++offset)
        {
            new (m_Elements + startElementIndex + offset) ElementType(Move(m_Elements[moveStartIndex + offset]));
            m_Elements[moveStartIndex + offset].~ElementType();
        }

        m_Count -= removeCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void RemoveIndexUnordered(usize elementIndex)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(elementIndex < m_Count);
        m_Elements[elementIndex].~ElementType();
        --m_Count;

        if (elementIndex != m_Count)
        {
            new (m_Elements + elementIndex) ElementType(Move(m_Elements[m_Count]));
            m_Elements[m_Count].~ElementType();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void RemoveIndicesUnordered(usize startElementIndex, usize removeCount)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        SE_ASSERT(startElementIndex + removeCount <= m_Count);

        for (usize offset = 0; offset < removeCount; ++offset)
            m_Elements[startElementIndex + offset].~ElementType();

        const usize remainingCount = m_Count - (startElementIndex + removeCount);
        usize moveCount = removeCount;
        if (moveCount > remainingCount)
            moveCount = remainingCount;
        const usize moveStartIndex = m_Count - moveCount;

        for (usize offset = 0; offset < moveCount; ++offset)
        {
            new (m_Elements + startElementIndex + offset) ElementType(Move(m_Elements[moveStartIndex + offset]));
            m_Elements[moveStartIndex + offset].~ElementType();
        }

        m_Count -= removeCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void Remove(const ElementType& element)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION
        
        usize elementIndex = 0;
        while (elementIndex < m_Count)
        {
            if (m_Elements[elementIndex] == element)
                RemoveIndex(elementIndex);
            else
                ++elementIndex;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void RemoveUnordered(const ElementType& element)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        usize elementIndex = 0;
        while (elementIndex < m_Count)
        {
            if (m_Elements[elementIndex] == element)
                RemoveIndexUnordered(elementIndex);
            else
                ++elementIndex;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void RemoveBuffered(const ElementType& element)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        Vector<ElementType, Allocator> newVector;
        newVector.ReAllocateElementsBuffer(m_Capacity);
        for (usize elementIndex = 0; elementIndex < m_Count; ++elementIndex)
        {
            if (m_Elements[elementIndex] == element)
                newVector.Add(Move(m_Elements[elementIndex]));
        }

        (*this) = newVector;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD FORCEINLINE bool Contains(const ElementType& element) const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        const Optional<usize> elementIndex = FindFirstElementIndex(element);
        return elementIndex.HasValue();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE Optional<usize> FindFirstElementIndex(const ElementType& element, usize searchFirstElementIndex = 0) const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        Optional<usize> elementIndex;
        for (usize index = searchFirstElementIndex; index < m_Count; ++index)
        {
            if (m_Elements[index] == element)
            {
                elementIndex = index;
                break;
            }
        }

        return elementIndex;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE void SetCountDefaulted(usize newCount)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        if (newCount == m_Count)
            return;

        if (newCount > m_Capacity)
        {
            ReAllocateElementsBuffer(newCount);
        }

        // Destroy excess elements. The loop condition already checks if any elements should be removed in the first place.
        for (usize elementIndex = newCount; elementIndex < m_Count; ++elementIndex)
            m_Elements[elementIndex].~ElementType();

        // Add missing elements. The loop condition already checks if any elements should be added in the first place.
        for (usize elementIndex = m_Count; elementIndex < newCount; ++elementIndex)
            new (m_Elements + elementIndex) ElementType();

        m_Count = newCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void SetCountFromTemplate(usize newCount, const ElementType& templateElement)
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        if (newCount == m_Count)
            return;

        if (newCount > m_Capacity)
        {
            ReAllocateElementsBuffer(newCount);
        }

        // Destroy excess elements. The loop condition already checks if any elements should be removed in the first place.
        for (usize elementIndex = newCount; elementIndex < m_Count; ++elementIndex)
            m_Elements[elementIndex].~ElementType();

        // Add missing elements. The loop condition already checks if any elements should be added in the first place.
        for (usize elementIndex = m_Count; elementIndex < newCount; ++elementIndex)
            new (m_Elements + elementIndex) ElementType(templateElement);

        m_Count = newCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void EnsureCapacity(usize requiredCapacity)
    {
        if (m_Capacity < requiredCapacity)
        {
            ReAllocateElementsBuffer(requiredCapacity);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE void Clear()
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        for (usize elementIndex = 0; elementIndex < m_Count; ++elementIndex)
            m_Elements[elementIndex].~ElementType();
        m_Count = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    FORCEINLINE void ShrinkToFit()
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_VECTOR_CHECK_PROTECTION

        if (m_Count == m_Capacity)
            return;

        ReAllocateElementsBuffer(m_Count);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    FORCEINLINE void ClearAndShrink()
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        Clear();

        Allocator::Release(m_Elements);
        m_Elements = nullptr;
        m_Capacity = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD FORCEINLINE ElementType* begin()
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        return m_Elements;
    }

    NODISCARD FORCEINLINE const ElementType* begin() const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        return m_Elements;
    }

    NODISCARD FORCEINLINE ElementType* end()
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        return m_Elements + m_Count;
    }

    NODISCARD FORCEINLINE const ElementType* end() const
    {
#if SE_VECTOR_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_VECTOR_CHECK_PROTECTION

        return m_Elements + m_Count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    NODISCARD FORCEINLINE static usize CalculateNextCapacity(usize currentCapacity, usize requiredCount)
    {
        const usize geometricNextCapacity = (currentCapacity * GROWTH_FACTOR_NUMERATOR) / GROWTH_FACTOR_DENOMINATOR;
        if (geometricNextCapacity >= requiredCount)
            return geometricNextCapacity;
        return requiredCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE static void CopyElements(ElementType* dstElements, const ElementType* srcElements, usize count)
    {
        for (usize elementIndex = 0; elementIndex < count; ++elementIndex)
            new (dstElements + elementIndex) ElementType(srcElements[elementIndex]);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE static void MoveElements(ElementType* dstElements, ElementType* srcElements, usize count)
    {
        for (usize elementIndex = 0; elementIndex < count; ++elementIndex)
        {
            new (dstElements + elementIndex) ElementType(Move(srcElements[elementIndex]));
            srcElements[elementIndex].~ElementType();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    FORCEINLINE void ReAllocateElementsBuffer(usize newCapacity)
    {
        // NOTE(Traian): This should never be triggered by the user incorrectly using the API.
        // If this assert fails, there is an internal container error.
        SE_ASSERT(newCapacity >= m_Count);

        // Allocate a new internal memory buffer.
        const usize allocationSize = newCapacity * sizeof(ElementType);
        void* newElementsBuffer = Allocator::GrowAllocation(m_Elements, allocationSize);
        ElementType* newElements = static_cast<ElementType*>(newElementsBuffer);

        // Only move the elements and destroy the old elements buffer if the grow operation returns
        // a different memory block address, otherwise there is no point (or undefined behaviour risk).
        if (m_Elements != newElements)
        {
            // Move the elements from the old buffer to the new buffer.
            Vector::MoveElements(newElements, m_Elements, m_Count);

            // Destroy the old internal memory buffer. Since the elements stored in it were
            // destryed after they were moved in the new buffer, no clean-up is required.
            Allocator::Release(m_Elements);
        }

        // Assign the new internal memory buffer.
        m_Elements = newElements;
        m_Capacity = newCapacity;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SE_VECTOR_CHECK_PROTECTION
    NODISCARD FORCEINLINE bool IsProtected() const
    {
        return (m_IsProtected == 1);
    }

    NODISCARD FORCEINLINE bool IsNotProtected() const
    {
        return (m_IsProtected == 0);
    }

    FORCEINLINE void SetIsProtected(bool isProtected) const
    {
        if (isProtected)
            m_IsProtected = 1;
        else
            m_IsProtected = 0;
    }
#endif // SE_VECTOR_CHECK_PROTECTION

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    ElementType* m_Elements;
    usize m_Capacity;
    uint64 m_Count : 63;
    mutable uint64 m_IsProtected : 1;
};

}
