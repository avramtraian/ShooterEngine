// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

template<typename T>
class Optional
{
public:
    FORCEINLINE Optional()
        : m_HasValue(false)
    {}

    FORCEINLINE Optional(const Optional& other)
        : m_HasValue(other.m_HasValue)
    {
        if (m_HasValue)
        {
            new (m_ValueBuffer) T(other.UncheckedValue());
        }
    }

    FORCEINLINE Optional(Optional&& other) noexcept
        : m_HasValue(other.m_HasValue)
    {
        if (m_HasValue)
        {
            new (m_ValueBuffer) T(Move(other.UncheckedValue()));
            other.UncheckedValue().~T();
            other.m_HasValue = false;
        }
    }

    FORCEINLINE Optional(const T& value)
        : m_HasValue(true)
    {
        new (m_ValueBuffer) T(value);
    }

    FORCEINLINE Optional(T&& value)
        : m_HasValue(true)
    {
        new (m_ValueBuffer) T(Move(value));
    }

    FORCEINLINE ~Optional()
    {
        Clear();
    }

    FORCEINLINE Optional& operator=(const Optional& other)
    {
        if (this == &other)
        {
            /* Handle the self-assignment case. */
            return *this;
        }

        Clear();
        m_HasValue = other.m_HasValue;

        if (m_HasValue)
        {
            new (m_ValueBuffer) T(other.UncheckedValue());
        }

        return *this;
    }

    FORCEINLINE Optional& operator=(Optional&& other) noexcept
    {
        if (this == &other)
        {
            /* Handle the self-assignment case. */
            return *this;
        }

        Clear();
        m_HasValue = other.m_HasValue;

        if (m_HasValue)
        {
            new (m_ValueBuffer) T(Move(other.UncheckedValue()));
            other.UncheckedValue().~T();
            other.m_HasValue = false;
        }

        return *this;
    }

    FORCEINLINE Optional& operator=(const T& value)
    {
        Clear();
        new (m_ValueBuffer) T(value);
        m_HasValue = true;
        return *this;
    }

    FORCEINLINE Optional& operator=(T&& value)
    {
        Clear();
        new (m_ValueBuffer) T(Move(value));
        m_HasValue = true;
        return *this;
    }

public:
    NODISCARD FORCEINLINE bool HasValue() const { return m_HasValue; }

    NODISCARD FORCEINLINE T& Value()
    {
        SE_CHECK(HasValue());
        return UncheckedValue();
    }

    NODISCARD FORCEINLINE const T& Value() const
    {
        SE_CHECK(HasValue());
        return UncheckedValue();
    }

    NODISCARD FORCEINLINE T& operator*() { return Value(); }
    NODISCARD FORCEINLINE const T& operator*() const { return Value(); }

    NODISCARD FORCEINLINE T* operator->() { return &Value(); }
    NODISCARD FORCEINLINE const T* operator->() const { return &Value(); }

    NODISCARD FORCEINLINE const T& ValueOr(const T& fallbackValue) const
    {
        if (m_HasValue)
            return UncheckedValue();
        return fallbackValue;
    }

public:
    FORCEINLINE void Clear()
    {
        if (m_HasValue)
        {
            UncheckedValue().~T();
            m_HasValue = false;
        }
    }

private:
    NODISCARD FORCEINLINE T& UncheckedValue() { return *reinterpret_cast<T*>(m_ValueBuffer); }
    NODISCARD FORCEINLINE const T& UncheckedValue() const { return *reinterpret_cast<const T*>(m_ValueBuffer); }

private:
    alignas(T) uint8 m_ValueBuffer[sizeof(T)];
    bool m_HasValue;
};

template<typename T>
class Optional<T&>
{
public:
    FORCEINLINE Optional()
        : m_Value(nullptr)
    {}

    FORCEINLINE ~Optional()
    {}

    FORCEINLINE Optional(const Optional& other)
        : m_Value(other.m_Value)
    {}

    FORCEINLINE Optional(Optional&& other) noexcept
        : m_Value(other.m_Value)
    {
        other.m_Value = nullptr;
    }

    FORCEINLINE Optional(T& value)
        : m_Value(&value)
    {}

    FORCEINLINE Optional& operator=(const Optional& other)
    {
        m_Value = other.m_Value;
        return *this;
    }

    FORCEINLINE Optional& operator=(Optional&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;
        
        m_Value = other.m_Value;
        other.m_Value = nullptr;

        return *this;
    }

    FORCEINLINE Optional& operator=(T& value)
    {
        m_Value = &value;
        return *this;
    }

public:
    NODISCARD FORCEINLINE bool HasValue() const
    {
        return (m_Value != nullptr);
    }

    NODISCARD FORCEINLINE T& Value()
    {
        SE_CHECK(HasValue());
        return *m_Value;
    }

    NODISCARD FORCEINLINE const T& Value() const
    {
        SE_CHECK(HasValue());
        return *m_Value;
    }

    NODISCARD FORCEINLINE T& NonConstValue() const
    {
        SE_CHECK(HasValue());
        return *m_Value;
    }

    NODISCARD FORCEINLINE T& operator*() { return Value(); }
    NODISCARD FORCEINLINE const T& operator*() const { return Value(); }

    NODISCARD FORCEINLINE T* operator->() { return &Value(); }
    NODISCARD FORCEINLINE const T* operator->() const { return &Value(); }

    NODISCARD FORCEINLINE const T& ValueOr(const T& fallbackValue) const
    {
        if (HasValue())
            return *m_Value;
        return fallbackValue;
    }

public:
    FORCEINLINE void Clear()
    {
        m_Value = nullptr;
    }

private:
    T* m_Value;
};

}
