// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class UUID
{
public:
    NODISCARD FORCEINLINE static UUID Invalid()
    {
        return UUID(0);
    }

    NODISCARD SHOOTER_API static UUID Generate();

    NODISCARD FORCEINLINE static uint64 GetHash(const UUID& uuid)
    {
        // NOTE(Traian): Since UUID values are randomly generated, unlike plain uint64's
        // the values have perfect bit entropy (which might offer better performance depending
        // on the implementation of the containers that use hashing).
        return uuid.GetValue();
    }

public:
    FORCEINLINE UUID()
        : m_Value(0)
    {}

    FORCEINLINE explicit UUID(uint64 value)
        : m_Value(value)
    {}

    FORCEINLINE UUID(const UUID& other)
        : m_Value(other.m_Value)
    {}

    FORCEINLINE UUID(UUID&& other) noexcept
        : m_Value(other.m_Value)
    {}

    FORCEINLINE UUID& operator=(const UUID& other)
    {
        m_Value = other.m_Value;
        return *this;
    }

    FORCEINLINE UUID& operator=(UUID&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_Value = other.m_Value;
        other.m_Value = 0;
        return *this;
    }

public:
    NODISCARD FORCEINLINE uint64 GetValue() const { return m_Value; }
    NODISCARD FORCEINLINE uint64 operator*() const { return m_Value; }

public:
    NODISCARD FORCEINLINE bool operator==(const UUID& other) const { return (m_Value == other.m_Value); }
    NODISCARD FORCEINLINE bool operator!=(const UUID& other) const { return (m_Value != other.m_Value); }

private:
    uint64 m_Value;
};

}

namespace std
{

template<>
struct hash<SE::UUID> {
    std::size_t operator()(const SE::UUID& uuid) const noexcept
    {
        return std::hash<uint64_t>{}(uuid.GetValue());
    }
};

}
