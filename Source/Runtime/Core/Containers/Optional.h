/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Assertions.h"
#include "Core/Memory/MemoryOperations.h"

namespace SE {

template<typename T>
class Optional {
public:
    ALWAYS_INLINE Optional()
        : m_has_value(false)
    {}

    ALWAYS_INLINE Optional(const Optional& other)
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
            new (m_value_storage) T(other.unchecked_value());
    }

    ALWAYS_INLINE Optional(Optional&& other) noexcept
        : m_has_value(other.m_has_value)
    {
        if (m_has_value) {
            new (m_value_storage) T(move(other.unchecked_value()));
            other.m_has_value = false;
            other.unchecked_value().~T();
        }
    }

    ALWAYS_INLINE Optional(const T& value)
        : m_has_value(true)
    {
        new (m_value_storage) T(value);
    }

    ALWAYS_INLINE Optional(T&& value)
        : m_has_value(true)
    {
        new (m_value_storage) T(move(value));
    }

    ALWAYS_INLINE ~Optional() { clear(); }

    ALWAYS_INLINE Optional& operator=(const Optional& other)
    {
        clear();

        if (other.m_has_value) {
            m_has_value = true;
            new (m_value_storage) T(other.unchecked_value());
        }

        return *this;
    }

    ALWAYS_INLINE Optional& operator=(Optional&& other) noexcept
    {
        clear();

        if (other.m_has_value) {
            m_has_value = true;
            new (m_value_storage) T(move(other.unchecked_value()));

            other.m_has_value = false;
            other.unchecked_value().~T();
        }

        return *this;
    }

    ALWAYS_INLINE Optional& operator=(const T& value)
    {
        clear();
        m_has_value = true;
        new (m_value_storage) T(value);
        return *this;
    }

    ALWAYS_INLINE Optional& operator=(T&& value)
    {
        clear();
        m_has_value = true;
        new (m_value_storage) T(move(value));
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool has_value() const { return m_has_value; }
    NODISCARD ALWAYS_INLINE operator bool() const { return has_value(); }

    NODISCARD ALWAYS_INLINE T& value()
    {
        SE_ASSERT(has_value());
        return unchecked_value();
    }

    NODISCARD ALWAYS_INLINE const T& value() const
    {
        SE_ASSERT(has_value());
        return unchecked_value();
    }

    NODISCARD ALWAYS_INLINE T& operator*() { return value(); }
    NODISCARD ALWAYS_INLINE const T& operator*() const { return value(); }

    NODISCARD ALWAYS_INLINE operator T() { return value(); }
    NODISCARD ALWAYS_INLINE operator const T() const { return value(); }

    NODISCARD ALWAYS_INLINE T& value_or(T& fallback_value) { return m_has_value ? unchecked_value() : fallback_value; }
    NODISCARD ALWAYS_INLINE const T& value_or(const T& fallback_value) const
    {
        return m_has_value ? unchecked_value() : fallback_value;
    }

    // Moves the held value to a new variable and clears the Optional.
    NODISCARD ALWAYS_INLINE T release_value()
    {
        T released_value = move(value());
        clear();
        return released_value;
    }

public:
    ALWAYS_INLINE void clear()
    {
        if (m_has_value) {
            m_has_value = false;
            unchecked_value().~T();
        }
    }

private:
    NODISCARD ALWAYS_INLINE T& unchecked_value() { return *reinterpret_cast<T*>(m_value_storage); }
    NODISCARD ALWAYS_INLINE const T& unchecked_value() const { return *reinterpret_cast<const T*>(m_value_storage); }

private:
    alignas(T) u8 m_value_storage[sizeof(T)];
    bool m_has_value;
};

template<typename T>
class Optional<T&> {
public:
    ALWAYS_INLINE Optional()
        : m_value(nullptr)
    {}

    ALWAYS_INLINE Optional(const Optional& other)
        : m_value(other.m_value)
    {}

    ALWAYS_INLINE Optional(Optional&& other) noexcept
        : m_value(other.m_value)
    {
        other.m_value = nullptr;
    }

    ALWAYS_INLINE Optional(T& value)
        : m_value(&value)
    {}

    ALWAYS_INLINE Optional& operator=(const Optional& other)
    {
        m_value = other.m_value;
        return *this;
    }

    ALWAYS_INLINE Optional& operator=(Optional&& other) noexcept
    {
        m_value = other.m_value;
        other.m_value = nullptr;
        return *this;
    }

    ALWAYS_INLINE Optional& operator=(T& value)
    {
        m_value = &value;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool has_value() const { return (m_value != nullptr); }
    NODISCARD ALWAYS_INLINE operator bool() const { return has_value(); }

    NODISCARD ALWAYS_INLINE T& value()
    {
        SE_ASSERT(has_value());
        return *m_value;
    }

    NODISCARD ALWAYS_INLINE const T& value() const
    {
        SE_ASSERT(has_value());
        return *m_value;
    }

    NODISCARD ALWAYS_INLINE T& operator*() { return value(); }
    NODISCARD ALWAYS_INLINE const T& operator*() const { return value(); }

    NODISCARD ALWAYS_INLINE operator T() { return value(); }
    NODISCARD ALWAYS_INLINE operator const T() const { return value(); }

    NODISCARD ALWAYS_INLINE T& value_or(T& fallback_value) { return has_value() ? *m_value : fallback_value; }
    NODISCARD ALWAYS_INLINE const T& value_or(const T& fallback_value) const
    {
        return has_value() ? *m_value : fallback_value;
    }

    // Moves the held value to a new variable and clears the Optional.
    NODISCARD ALWAYS_INLINE T& release_value()
    {
        T& released_value = value();
        clear();
        return released_value;
    }

public:
    ALWAYS_INLINE void clear() { m_value = nullptr; }

private:
    T* m_value;
};

} // namespace SE