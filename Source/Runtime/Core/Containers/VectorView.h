// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

#include <vector>

namespace SE
{

template<typename T>
class VectorView
{
public:
    using NonConstT = std::remove_const_t<T>;

    using Iterator = T*;
    using ConstIterator = const T*;

public:
    FORCEINLINE constexpr VectorView()
        : m_Elements(nullptr)
        , m_Count(0)
    {}

    FORCEINLINE constexpr VectorView(const VectorView& other)
        : m_Elements(other.m_Elements)
        , m_Count(other.m_Count)
    {}

    FORCEINLINE constexpr VectorView(VectorView&& other) noexcept
        : m_Elements(other.m_Elements)
        , m_Count(other.m_Count)
    {
        other.m_Elements = nullptr;
        other.m_Count = 0;
    }

    FORCEINLINE constexpr VectorView& operator=(const VectorView& other)
    {
        // Handle self-assignment case.
        if (this == &other)
            return *this;

        m_Elements = other.m_Elements;
        m_Count = other.m_Count;
        return *this;
    }

    FORCEINLINE constexpr VectorView& operator=(VectorView&& other) noexcept
    {
        // Handle self-assignment case.
        if (this == &other)
            return *this;

        m_Elements = other.m_Elements;
        m_Count = other.m_Count;
        other.m_Elements = nullptr;
        other.m_Count = 0;
        return *this;
    }

public:
    FORCEINLINE constexpr VectorView(T* elements, usize count)
        : m_Elements(elements)
        , m_Count(count)
    {}

    FORCEINLINE VectorView(const std::vector<NonConstT>& vector)
        : m_Elements(vector.data())
        , m_Count(vector.size())
    {}

    FORCEINLINE VectorView(std::vector<NonConstT>& vector)
        : m_Elements(vector.data())
        , m_Count(vector.size())
    {}

    FORCEINLINE VectorView& operator=(const std::vector<NonConstT>& vector)
    {
        m_Elements = vector.data();
        m_Count = vector.size();
        return *this;
    }

    FORCEINLINE VectorView& operator=(std::vector<NonConstT>& vector)
    {
        m_Elements = vector.data();
        m_Count = vector.size();
        return *this;
    }

public:
    NODISCARD FORCEINLINE T* Elements() { return m_Elements; }
    NODISCARD FORCEINLINE const T* Elements() const { return m_Elements; }

    NODISCARD FORCEINLINE usize Count() const { return m_Count; }
    NODISCARD FORCEINLINE bool IsEmpty() const { return (m_Count == 0); }
    NODISCARD FORCEINLINE bool HasElements() const { return (m_Count > 0); }

public:
    NODISCARD FORCEINLINE T& At(usize index)
    {
        SE_ASSERT(index < m_Count);
        return m_Elements[index];
    }

    NODISCARD FORCEINLINE const T& At(usize index) const
    {
        SE_ASSERT(index < m_Count);
        return m_Elements[index];
    }

    NODISCARD FORCEINLINE T& operator[](usize index) { return At(index); }
    NODISCARD FORCEINLINE const T& operator[](usize index) const { return At(index); }

    NODISCARD FORCEINLINE T& First()
    {
        SE_ASSERT(HasElements());
        return m_Elements[0];
    }

    NODISCARD FORCEINLINE const T& First() const
    {
        SE_ASSERT(HasElements());
        return m_Elements[0];
    }

    NODISCARD FORCEINLINE T& Last()
    {
        SE_ASSERT(HasElements());
        return m_Elements[m_Count - 1];
    }

    NODISCARD FORCEINLINE const T& Last() const
    {
        SE_ASSERT(HasElements());
        return m_Elements[m_Count - 1];
    }

public:
    NODISCARD FORCEINLINE std::vector<NonConstT> ToVector() const
    {
        std::vector<NonConstT> vector;
        vector.reserve(m_Count);
        for (usize index = 0; index < m_Count; ++index)
            vector.push_back(m_Elements[index]);
        return vector;
    }

public:
    FORCEINLINE NODISCARD Iterator begin() { return Iterator(m_Elements); }
    FORCEINLINE NODISCARD Iterator end() { return Iterator(m_Elements + m_Count); }

    FORCEINLINE NODISCARD ConstIterator begin() const { return ConstIterator(m_Elements); }
    FORCEINLINE NODISCARD ConstIterator end() const { return ConstIterator(m_Elements + m_Count); }

private:
    T* m_Elements;
    usize m_Count;
};

}
