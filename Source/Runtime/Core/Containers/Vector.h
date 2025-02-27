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
    static constexpr usize growth_factor_numerator = 3;
    static constexpr usize growth_factor_denominator = 2;

    using Iterator             = T*;
    using ConstIterator        = const T*;
    using ReverseIterator      = T*;
    using ReverseConstIterator = const T*;

public:
    FORCEINLINE Vector()
        : m_elements(nullptr)
        , m_capacity(0)
        , m_count(0)
    {}

    FORCEINLINE Vector(const Vector& other)
        : m_capacity(other.m_count)
        , m_count(other.m_count)
    {
        m_elements = Vector::allocate_memory(m_capacity);
        Vector::copy_elements(m_elements, other.m_elements, m_count);
    }

    FORCEINLINE Vector(Vector&& other) noexcept
        : m_elements(other.m_elements)
        , m_capacity(other.m_capacity)
        , m_count(other.m_count)
    {
        other.m_elements = nullptr;
        other.m_capacity = 0;
        other.m_count = 0;
    }

    FORCEINLINE Vector(std::initializer_list<T> init_list)
        : m_capacity(init_list.size())
        , m_count(init_list.size())
    {
        m_elements = allocate_memory(m_capacity);
        copy_elements(m_elements, init_list.begin(), m_count);
    }

    FORCEINLINE ~Vector()
    {
        clear_and_shrink();
    }

    FORCEINLINE Vector& operator=(const Vector& other)
    {
        if (this == &other)
        {
            /* Handle self-assignment case. */
            return *this;
        }

        clear();
        ensure_capacity(other.m_count);

        m_count = other.m_count;
        copy_elements(m_elements, other.m_elements, m_count);

        return *this;
    }

    FORCEINLINE Vector& operator=(Vector&& other) noexcept
    {
        if (this == &other)
        {
            // Handle self-assignment case.
            return *this;
        }

        clear_and_shrink();

        m_elements = other.m_elements;
        m_capacity = other.m_capacity;
        m_count = other.m_count;

        other.m_elements = nullptr;
        other.m_capacity = 0;
        other.m_count = 0;

        return *this;
    }

    FORCEINLINE Vector& operator=(std::initializer_list<T> init_list)
    {
        clear();
        ensure_capacity(init_list.size());

        m_count = init_list.size();
        copy_elements(m_elements, init_list.begin(), m_count);

        return *this;
    }

public:
    NODISCARD FORCEINLINE T* elements() { return m_elements; }
    NODISCARD FORCEINLINE const T* elements() const { return m_elements; }

    NODISCARD FORCEINLINE usize capacity() const { return m_capacity; }
    NODISCARD FORCEINLINE usize count() const { return m_count; }

    NODISCARD FORCEINLINE bool is_empty() const { return m_count == 0; }
    NODISCARD FORCEINLINE bool has_elements() const { return m_count > 0; }

public:
    NODISCARD FORCEINLINE T& at(usize index)
    {
        /* Index is out of bounds. */
        SE_CHECK(index < m_count);
        return m_elements[index];
    }

    NODISCARD FORCEINLINE const T& at(usize index) const
    {
        /* Index is out of bounds. */
        SE_CHECK(index < m_count);
        return m_elements[index];
    }

    NODISCARD FORCEINLINE T&       first()       { SE_CHECK(has_elements()); return m_elements[0]; }
    NODISCARD FORCEINLINE const T& first() const { SE_CHECK(has_elements()); return m_elements[0]; }
    NODISCARD FORCEINLINE T&       last()        { SE_CHECK(has_elements()); return m_elements[m_count - 1]; }
    NODISCARD FORCEINLINE const T& last() const  { SE_CHECK(has_elements()); return m_elements[m_count - 1]; }

public:
    FORCEINLINE void add(const T& element)
    {
        ensure_capacity(m_count + 1);
        new (m_elements + m_count) T(element);
        ++m_count;
    }

    FORCEINLINE void add(T&& element)
    {
        ensure_capacity(m_count + 1);
        new (m_elements + m_count) T(move(element));
        ++m_count;
    }

    template<typename... Args>
    FORCEINLINE void emplace(Args&&... args)
    {
        ensure_capacity(m_count + 1);
        new (m_elements + m_count) T(forward<Args>(args)...);
        ++m_count;
    }

    FORCEINLINE void add(std::initializer_list<T> init_list)
    {
        ensure_capacity(m_count + init_list.size());
        
        for (usize index = m_count; index < m_count + init_list.size(); ++index)
        {
            new (m_elements + index) T(init_list.begin()[index]);
        }

        m_count += init_list.size();
    }

public:
    /**
     * Removes the element stored at the given index by calling its destructor and moving the last
     * element stored in the vector on its slot. 
     * By performing this swap, the operation time complexity remains O(1), but the elements order
     * will not be conserved.
     */
    FORCEINLINE void remove_index_unordered(usize element_index)
    {
        /* Index is out of bounds. */
        SE_CHECK(element_index < m_count);

        m_elements[element_index].~T();
        --m_count;

        if (element_index != m_count)
        {
            new (m_elements + element_index) T(m_elements[m_count]);
            m_elements[m_count].~T();
        }
    }

public:
    /* Returns 'invalid_index' if the vector doesn't contain the provided element. */
    FORCEINLINE usize find_index_of(const T& element) const
    {
        for (usize index = 0; index < m_count; ++index)
        {
            if (m_elements[index] == element)
            {
                return index;
            }
        }

        return invalid_index;
    }

    FORCEINLINE bool contains(const T& element) const
    {
        const usize element_index = find_index_of(element);
        return element_index != invalid_index;
    }

public:
    FORCEINLINE void clear()
    {
        for (usize index = 0; index < m_count; ++index)
        {
            m_elements[index].~T();
        }

        m_count = 0;
    }
    FORCEINLINE void clear_and_shrink()
    {
        clear();
        Vector::free_memory(m_elements, m_capacity);
        m_elements = nullptr;
        m_capacity = 0;
    }
     
    FORCEINLINE void ensure_capacity(usize required_capacity)
    {
        if (required_capacity <= m_capacity)
        {
            /* No expansion is needed. */
            return;
        }

        usize new_capacity = (m_capacity * growth_factor_numerator) / growth_factor_denominator;
        if (new_capacity < required_capacity)
        {
            /* The default (geometric) expansion size is not sufficient for storing the
             * required number of elements. */
            new_capacity = required_capacity;
        }

        T* new_elements = Vector::allocate_memory(new_capacity);
        move_elements(new_elements, m_elements, m_count);
        Vector::free_memory(m_elements, m_capacity);

        m_elements = new_elements;
        m_capacity = new_capacity;
    }

public:
    NODISCARD FORCEINLINE Iterator begin() { return Iterator(m_elements); }
    NODISCARD FORCEINLINE Iterator end() { return Iterator(m_elements + m_count); }

    NODISCARD FORCEINLINE ConstIterator begin() const { return ConstIterator(m_elements); }
    NODISCARD FORCEINLINE ConstIterator end() const { return ConstIterator(m_elements + m_count); }

    NODISCARD FORCEINLINE ReverseIterator rbegin() { return ReverseIterator(m_elements + m_count - 1); }
    NODISCARD FORCEINLINE ReverseIterator rend() { return ReverseIterator(m_elements - 1); }

    NODISCARD FORCEINLINE ReverseConstIterator rbegin() const { return ReverseConstIterator(m_elements + m_count - 1); }
    NODISCARD FORCEINLINE ReverseConstIterator rend() const { return ReverseConstIterator(m_elements - 1); }

private:
    FORCEINLINE static T* allocate_memory(usize in_capacity)
    {
        if (in_capacity == 0)
        {
            return nullptr;
        }

        return static_cast<T*>(::operator new(in_capacity * sizeof(T)));
    }

    FORCEINLINE static void free_memory(T* in_elements, usize in_capacity)
    {
        if (in_capacity == 0)
        {
            return;
        }

        ::operator delete(in_elements);
    }

    FORCEINLINE static void copy_elements(T* destination_elements, const T* source_elements, usize in_count)
    {
        for (usize index = 0; index < in_count; ++index)
        {
            new (destination_elements + index) T(source_elements[index]);
        }
    }

    FORCEINLINE static void move_elements(T* destination_elements, T* source_elements, usize in_count)
    {
        for (usize index = 0; index < in_count; ++index)
        {
            new (destination_elements + index) T(move(source_elements[index]));
            source_elements[index].~T();
        }
    }

private:
    T* m_elements;
    usize m_capacity;
    usize m_count;
};

}
