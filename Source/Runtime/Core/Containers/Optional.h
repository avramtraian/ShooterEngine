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
        : m_has_value(false)
    {}

    FORCEINLINE Optional(const Optional& other)
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            new (m_value_buffer) T(other.unchecked_value());
        }
    }

    FORCEINLINE Optional(Optional&& other) noexcept
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            new (m_value_buffer) T(move(other.unchecked_value()));
            other.unchecked_value().~T();
            other.m_has_value = false;
        }
    }

    FORCEINLINE Optional(const T& value)
        : m_has_value(true)
    {
        new (m_value_buffer) T(value);
    }

    FORCEINLINE Optional(T&& value)
        : m_has_value(true)
    {
        new (m_value_buffer) T(move(value));
    }

    FORCEINLINE ~Optional()
    {
        clear();
    }

    FORCEINLINE Optional& operator=(const Optional& other)
    {
        if (this == &other)
        {
            /* Handle the self-assignment case. */
            return *this;
        }

        clear();
        m_has_value = other.m_has_value;

        if (m_has_value)
        {
            new (m_value_buffer) T(other.unchecked_value());
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

        clear();
        m_has_value = other.m_has_value;

        if (m_has_value)
        {
            new (m_value_buffer) T(move(other.unchecked_value()));
            other.unchecked_value().~T();
            other.m_has_value = false;
        }

        return *this;
    }

    FORCEINLINE Optional& operator=(const T& value)
    {
        clear();
        new (m_value_buffer) T(value);
        m_has_value = true;
        return *this;
    }

    FORCEINLINE Optional& operator=(T&& value)
    {
        clear();
        new (m_value_buffer) T(move(value));
        m_has_value = true;
        return *this;
    }

public:
    NODISCARD FORCEINLINE bool has_value() const { return m_has_value; }

    NODISCARD FORCEINLINE T& value()
    {
        SE_CHECK(has_value());
        return unchecked_value();
    }

    NODISCARD FORCEINLINE const T& value() const
    {
        SE_CHECK(has_value());
        return unchecked_value();
    }

    NODISCARD FORCEINLINE T& operator*() { return value(); }
    NODISCARD FORCEINLINE const T& operator*() const { return value(); }

    NODISCARD FORCEINLINE T* operator->() { return &value(); }
    NODISCARD FORCEINLINE const T* operator->() const { return &value(); }

    NODISCARD FORCEINLINE const T& value_or(const T& fallback_value) const
    {
        if (m_has_value)
        {
            return unchecked_value();
        }

        return fallback_value;
    }

public:
    FORCEINLINE void clear()
    {
        if (m_has_value)
        {
            unchecked_value().~T();
            m_has_value = false;
        }
    }

private:
    NODISCARD FORCEINLINE T& unchecked_value() { return *reinterpret_cast<T*>(m_value_buffer); }
    NODISCARD FORCEINLINE const T& unchecked_value() const { return *reinterpret_cast<const T*>(m_value_buffer); }

private:
    alignas(T) uint8 m_value_buffer[sizeof(T)];
    bool m_has_value;
};

}
