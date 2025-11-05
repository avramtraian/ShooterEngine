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
    ALWAYS_INLINE Optional()
        : m_HasValue(false)
    {}

    ALWAYS_INLINE Optional(const Optional& other)
        : m_HasValue(other.m_HasValue)
    {
        if (m_HasValue)
        {
            new (m_ValueBuffer) T(other.UncheckedValue());
        }
    }

    ALWAYS_INLINE Optional(Optional&& other) noexcept
        : m_HasValue(other.m_HasValue)
    {
        if (m_HasValue)
        {
            new (m_ValueBuffer) T(Move(other.UncheckedValue()));
            other.UncheckedValue().~T();
            other.m_HasValue = false;
        }
    }

    ALWAYS_INLINE Optional(const T& value)
        : m_HasValue(true)
    {
        new (m_ValueBuffer) T(value);
    }

    ALWAYS_INLINE Optional(T&& value)
        : m_HasValue(true)
    {
        new (m_ValueBuffer) T(Move(value));
    }

    ALWAYS_INLINE ~Optional()
    {
        Clear();
    }

    ALWAYS_INLINE Optional& operator=(const Optional& other)
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

    ALWAYS_INLINE Optional& operator=(Optional&& other) noexcept
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

    ALWAYS_INLINE Optional& operator=(const T& value)
    {
        Clear();
        new (m_ValueBuffer) T(value);
        m_HasValue = true;
        return *this;
    }

    ALWAYS_INLINE Optional& operator=(T&& value)
    {
        Clear();
        new (m_ValueBuffer) T(Move(value));
        m_HasValue = true;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool HasValue() const { return m_HasValue; }

    NODISCARD ALWAYS_INLINE T& Value()
    {
        SE_CHECK(HasValue());
        return UncheckedValue();
    }

    NODISCARD ALWAYS_INLINE const T& Value() const
    {
        SE_CHECK(HasValue());
        return UncheckedValue();
    }

    NODISCARD ALWAYS_INLINE T& operator*() { return Value(); }
    NODISCARD ALWAYS_INLINE const T& operator*() const { return Value(); }

    NODISCARD ALWAYS_INLINE T* operator->() { return &Value(); }
    NODISCARD ALWAYS_INLINE const T* operator->() const { return &Value(); }

    NODISCARD ALWAYS_INLINE T ValueOr(const T& fallbackValue) const
    {
        if (m_HasValue)
            return UncheckedValue();
        return fallbackValue;
    }

public:
    ALWAYS_INLINE void Clear()
    {
        if (m_HasValue)
        {
            UncheckedValue().~T();
            m_HasValue = false;
        }
    }

private:
    NODISCARD ALWAYS_INLINE T& UncheckedValue() { return *reinterpret_cast<T*>(m_ValueBuffer); }
    NODISCARD ALWAYS_INLINE const T& UncheckedValue() const { return *reinterpret_cast<const T*>(m_ValueBuffer); }

private:
    alignas(T) uint8 m_ValueBuffer[sizeof(T)];
    bool m_HasValue;
};

template<typename T>
class Optional<T&>
{
public:
    ALWAYS_INLINE Optional()
        : m_Value(nullptr)
    {}

    ALWAYS_INLINE ~Optional()
    {}

    ALWAYS_INLINE Optional(const Optional& other)
        : m_Value(other.m_Value)
    {}

    ALWAYS_INLINE Optional(Optional&& other) noexcept
        : m_Value(other.m_Value)
    {
        other.m_Value = nullptr;
    }

    ALWAYS_INLINE Optional(T& value)
        : m_Value(&value)
    {}

    ALWAYS_INLINE Optional& operator=(const Optional& other)
    {
        m_Value = other.m_Value;
        return *this;
    }

    ALWAYS_INLINE Optional& operator=(Optional&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;
        
        m_Value = other.m_Value;
        other.m_Value = nullptr;

        return *this;
    }

    ALWAYS_INLINE Optional& operator=(T& value)
    {
        m_Value = &value;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool HasValue() const
    {
        return (m_Value != nullptr);
    }

    NODISCARD ALWAYS_INLINE T& Value()
    {
        SE_CHECK(HasValue());
        return *m_Value;
    }

    NODISCARD ALWAYS_INLINE const T& Value() const
    {
        SE_CHECK(HasValue());
        return *m_Value;
    }

    NODISCARD ALWAYS_INLINE T& NonConstValue() const
    {
        SE_CHECK(HasValue());
        return *m_Value;
    }

    NODISCARD ALWAYS_INLINE T& operator*() { return Value(); }
    NODISCARD ALWAYS_INLINE const T& operator*() const { return Value(); }

    NODISCARD ALWAYS_INLINE T* operator->() { return &Value(); }
    NODISCARD ALWAYS_INLINE const T* operator->() const { return &Value(); }

    NODISCARD ALWAYS_INLINE T ValueOr(const T& fallbackValue) const
    {
        if (HasValue())
            return *m_Value;
        return fallbackValue;
    }

public:
    ALWAYS_INLINE void Clear()
    {
        m_Value = nullptr;
    }

private:
    T* m_Value;
};

}
