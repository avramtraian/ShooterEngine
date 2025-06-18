// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

template<typename T>
class Vector
{
public:
    /* By default, the container expands its internal memory block by a growth factor of 1.5. */
    static constexpr usize GROWTH_FACTOR_NUMERATOR = 3;
    static constexpr usize GROWTH_FACTOR_DENOMINATOR = 2;

    using Iterator             = T*;
    using ConstIterator        = const T*;
    using ReverseIterator      = T*;
    using ReverseConstIterator = const T*;

public:
    FORCEINLINE Vector()
        : m_Elements(nullptr)
        , m_Capacity(0)
        , m_Count(0)
    {}

    FORCEINLINE Vector(const Vector& other)
        : m_Capacity(other.m_Count)
        , m_Count(other.m_Count)
    {
        m_Elements = Vector::AllocateMemory(m_Capacity);
        Vector::CopyElements(m_Elements, other.m_Elements, m_Count);
    }

    FORCEINLINE Vector(Vector&& other) noexcept
        : m_Elements(other.m_Elements)
        , m_Capacity(other.m_Capacity)
        , m_Count(other.m_Count)
    {
        other.m_Elements = nullptr;
        other.m_Capacity = 0;
        other.m_Count = 0;
    }

    FORCEINLINE Vector(std::initializer_list<T> initializerList)
        : m_Capacity(initializerList.size())
        , m_Count(initializerList.size())
    {
        m_Elements = AllocateMemory(m_Capacity);
        CopyElements(m_Elements, initializerList.begin(), m_Count);
    }

    FORCEINLINE ~Vector()
    {
        ClearAndShrink();
    }

    FORCEINLINE Vector& operator=(const Vector& other)
    {
        if (this == &other)
        {
            /* Handle self-assignment case. */
            return *this;
        }

        Clear();
        EnsureCapacity(other.m_Count);

        m_Count = other.m_Count;
        CopyElements(m_Elements, other.m_Elements, m_Count);

        return *this;
    }

    FORCEINLINE Vector& operator=(Vector&& other) noexcept
    {
        if (this == &other)
        {
            // Handle self-assignment case.
            return *this;
        }

        ClearAndShrink();

        m_Elements = other.m_Elements;
        m_Capacity = other.m_Capacity;
        m_Count = other.m_Count;

        other.m_Elements = nullptr;
        other.m_Capacity = 0;
        other.m_Count = 0;

        return *this;
    }

    FORCEINLINE Vector& operator=(std::initializer_list<T> initializerList)
    {
        Clear();
        EnsureCapacity(initializerList.size());

        m_Count = initializerList.size();
        CopyElements(m_Elements, initializerList.begin(), m_Count);

        return *this;
    }

public:
    NODISCARD FORCEINLINE T* Elements() { return m_Elements; }
    NODISCARD FORCEINLINE const T* Elements() const { return m_Elements; }

    NODISCARD FORCEINLINE usize Capacity() const { return m_Capacity; }
    NODISCARD FORCEINLINE usize Count() const { return m_Count; }

    NODISCARD FORCEINLINE bool IsEmpty() const { return m_Count == 0; }
    NODISCARD FORCEINLINE bool HasElements() const { return m_Count > 0; }

public:
    NODISCARD FORCEINLINE T& At(usize index)
    {
        /* Index is out of bounds. */
        SE_CHECK(index < m_Count);
        return m_Elements[index];
    }

    NODISCARD FORCEINLINE const T& At(usize index) const
    {
        /* Index is out of bounds. */
        SE_CHECK(index < m_Count);
        return m_Elements[index];
    }

    NODISCARD FORCEINLINE T&       First()       { SE_CHECK(HasElements()); return m_Elements[0]; }
    NODISCARD FORCEINLINE const T& First() const { SE_CHECK(HasElements()); return m_Elements[0]; }
    NODISCARD FORCEINLINE T&       Last()        { SE_CHECK(HasElements()); return m_Elements[m_Count - 1]; }
    NODISCARD FORCEINLINE const T& Last() const  { SE_CHECK(HasElements()); return m_Elements[m_Count - 1]; }

public:
    FORCEINLINE void Add(const T& element)
    {
        EnsureCapacity(m_Count + 1);
        new (m_Elements + m_Count) T(element);
        ++m_Count;
    }

    FORCEINLINE void Add(T&& element)
    {
        EnsureCapacity(m_Count + 1);
        new (m_Elements + m_Count) T(Move(element));
        ++m_Count;
    }

    template<typename... Args>
    FORCEINLINE void Emplace(Args&&... args)
    {
        EnsureCapacity(m_Count + 1);
        new (m_Elements + m_Count) T(Forward<Args>(args)...);
        ++m_Count;
    }

    FORCEINLINE void Add(std::initializer_list<T> initializerList)
    {
        EnsureCapacity(m_Count + initializerList.size());
        
        for (usize index = m_Count; index < m_Count + initializerList.size(); ++index)
            new (m_Elements + index) T(initializerList.begin()[index]);

        m_Count += initializerList.size();
    }

public:
    /**
     * Removes the element stored at the given index by calling its destructor and moving the last
     * element stored in the vector on its slot. 
     * By performing this swap, the operation time complexity remains O(1), but the elements order
     * will not be conserved.
     */
    FORCEINLINE void RemoveIndexUnordered(usize elementIndex)
    {
        /* Index is out of bounds. */
        SE_CHECK(elementIndex < m_Count);

        m_Elements[elementIndex].~T();
        --m_Count;

        if (elementIndex != m_Count)
        {
            new (m_Elements + elementIndex) T(m_Elements[m_Count]);
            m_Elements[m_Count].~T();
        }
    }

public:
    /* Returns 'INVALID_INDEX' if the vector doesn't contain the provided element. */
    FORCEINLINE usize FindIndexOf(const T& element) const
    {
        for (usize index = 0; index < m_Count; ++index)
        {
            if (m_Elements[index] == element)
                return index;
        }

        return INVALID_INDEX;
    }

    FORCEINLINE bool Contains(const T& element) const
    {
        const usize elementIndex = FindIndexOf(element);
        return elementIndex != INVALID_INDEX;
    }

public:
    FORCEINLINE void Clear()
    {
        for (usize index = 0; index < m_Count; ++index)
            m_Elements[index].~T();
        m_Count = 0;
    }

    FORCEINLINE void ClearAndShrink()
    {
        Clear();
        Vector::FreeMemory(m_Elements, m_Capacity);
        m_Elements = nullptr;
        m_Capacity = 0;
    }
     
    FORCEINLINE void EnsureCapacity(usize requiredCapacity)
    {
        if (requiredCapacity <= m_Capacity)
        {
            /* No expansion is needed. */
            return;
        }

        usize newCapacity = (m_Capacity * GROWTH_FACTOR_NUMERATOR) / GROWTH_FACTOR_DENOMINATOR;
        if (newCapacity < requiredCapacity)
        {
            /* The default (geometric) expansion size is not sufficient for storing the
             * required number of elements. */
            newCapacity = requiredCapacity;
        }

        T* newElements = Vector::AllocateMemory(newCapacity);
        MoveElements(newElements, m_Elements, m_Count);
        Vector::FreeMemory(m_Elements, m_Capacity);

        m_Elements = newElements;
        m_Capacity = newCapacity;
    }

public:
    NODISCARD FORCEINLINE Iterator begin() { return Iterator(m_Elements); }
    NODISCARD FORCEINLINE Iterator end() { return Iterator(m_Elements + m_Count); }

    NODISCARD FORCEINLINE ConstIterator begin() const { return ConstIterator(m_Elements); }
    NODISCARD FORCEINLINE ConstIterator end() const { return ConstIterator(m_Elements + m_Count); }

    NODISCARD FORCEINLINE ReverseIterator rbegin() { return ReverseIterator(m_Elements + m_Count - 1); }
    NODISCARD FORCEINLINE ReverseIterator rend() { return ReverseIterator(m_Elements - 1); }

    NODISCARD FORCEINLINE ReverseConstIterator rbegin() const { return ReverseConstIterator(m_Elements + m_Count - 1); }
    NODISCARD FORCEINLINE ReverseConstIterator rend() const { return ReverseConstIterator(m_Elements - 1); }

private:
    FORCEINLINE static T* AllocateMemory(usize capacity)
    {
        if (capacity == 0)
            return nullptr;
        return static_cast<T*>(::operator new(capacity * sizeof(T)));
    }

    FORCEINLINE static void FreeMemory(T* elements, usize capacity)
    {
        if (capacity == 0)
            return;
        ::operator delete(elements);
    }

    FORCEINLINE static void CopyElements(T* destinationElements, const T* sourceElements, usize count)
    {
        for (usize index = 0; index < count; ++index)
            new (destinationElements + index) T(sourceElements[index]);
    }

    FORCEINLINE static void MoveElements(T* destinationElements, T* sourceElements, usize count)
    {
        for (usize index = 0; index < count; ++index)
        {
            new (destinationElements + index) T(Move(sourceElements[index]));
            sourceElements[index].~T();
        }
    }

private:
    T* m_Elements;
    usize m_Capacity;
    usize m_Count;
};

}
