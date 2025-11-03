// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class UUID
{
public:
    NODISCARD ALWAYS_INLINE static constexpr UUID Invalid() { return UUID(0); }

    NODISCARD RUNTIME_API static UUID Generate();

    NODISCARD ALWAYS_INLINE static constexpr uint64 GetHash(const UUID& uuid)
    {
        // NOTE(Traian): Since UUID values are randomly generated, unlike plain uint64's
        // the values have perfect bit entropy (which might offer better performance depending
        // on the implementation of the containers that use hashing).
        return uuid.GetValue();
    }

public:
    ALWAYS_INLINE UUID()
        : m_Value(0)
    {}

    ALWAYS_INLINE explicit constexpr UUID(uint64 value)
        : m_Value(value)
    {}

    ALWAYS_INLINE constexpr UUID(const UUID& other)
        : m_Value(other.m_Value)
    {}

    ALWAYS_INLINE constexpr UUID(UUID&& other) noexcept
        : m_Value(other.m_Value)
    {}

    ALWAYS_INLINE constexpr UUID& operator=(const UUID& other)
    {
        m_Value = other.m_Value;
        return *this;
    }

    ALWAYS_INLINE constexpr UUID& operator=(UUID&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_Value       = other.m_Value;
        other.m_Value = 0;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE constexpr uint64 GetValue() const { return m_Value; }
    NODISCARD ALWAYS_INLINE constexpr uint64 operator*() const { return m_Value; }

public:
    NODISCARD ALWAYS_INLINE constexpr bool operator==(const UUID& other) const { return (m_Value == other.m_Value); }
    NODISCARD ALWAYS_INLINE constexpr bool operator!=(const UUID& other) const { return (m_Value != other.m_Value); }

private:
    uint64 m_Value;
};

} // namespace SE
