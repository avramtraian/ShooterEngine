// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

template<typename T>
class VectorView
{
public:
    using Iterator = T*;
    using ConstIterator = const T*;

public:
    FORCEINLINE constexpr VectorView()
        : m_Elements(nullptr)
        , m_Count(0)
    {}

    constexpr VectorView(const VectorView&) = default;
    constexpr VectorView(VectorView&&) noexcept = default;

    constexpr VectorView& operator=(const VectorView&) = default;
    constexpr VectorView& operator=(VectorView&&) noexcept = default;

    FORCEINLINE constexpr VectorView(T* elements, usize count)
        : m_Elements(elements)
        , m_Count(count)
    {}

public:
    FORCEINLINE constexpr VectorView(const VectorView<RemoveConst<T>>& other)
    requires (!std::is_const_v<T>)
        : m_Elements(other.m_Elements)
        , m_Count(other.m_Count)
    {}

    FORCEINLINE constexpr VectorView& operator=(const VectorView<RemoveConst<T>>& other)
    requires (!std::is_const_v<T>)
    {
        m_Elements = other.m_Elements;
        m_Count = other.m_Count;
        return *this;
    }

public:
    NODISCARD FORCEINLINE T* Elements() { return m_Elements; }
    NODISCARD FORCEINLINE const T* Elements() const { return m_Elements; }

    NODISCARD FORCEINLINE usize Count() const { return m_Count; }
    NODISCARD FORCEINLINE bool IsEmpty() const { return (m_Count == 0); }
    NODISCARD FORCEINLINE bool HasElements() const { return (m_Count > 0); }

public:
    NODISCARD FORCEINLINE uint8* Bytes() { return (uint8*)m_Elements; }
    NODISCARD FORCEINLINE const uint8* Bytes() const { return (const uint8*)m_Elements; }

    NODISCARD FORCEINLINE usize ByteCount() const { return m_Count * sizeof(T); }

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
    FORCEINLINE NODISCARD Iterator begin() { return Iterator(m_Elements); }
    FORCEINLINE NODISCARD Iterator end() { return Iterator(m_Elements + m_Count); }

    FORCEINLINE NODISCARD ConstIterator begin() const { return ConstIterator(m_Elements); }
    FORCEINLINE NODISCARD ConstIterator end() const { return ConstIterator(m_Elements + m_Count); }

private:
    T* m_Elements;
    usize m_Count;
};

template<typename T>
using ConstVectorView = VectorView<const T>;

}
