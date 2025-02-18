// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class UUID
{
public:
    // Generates a new unique UUID using the math random function.
    // There is no guarantee that the generated UUID value is actually unique, but the
    // chance that it is a duplicate is so small that it is not worth it trying to avoid it.
    NODISCARD SHOOTER_API static UUID create();

    NODISCARD ALWAYS_INLINE static constexpr UUID invalid()
    {
        return UUID(0);
    }

public:
    ALWAYS_INLINE constexpr UUID()
        : m_uuid_value(0)
    {}

    ALWAYS_INLINE constexpr UUID(const UUID& other)
        : m_uuid_value(other.m_uuid_value)
    {}

    ALWAYS_INLINE constexpr explicit UUID(u64 uuid_value)
        : m_uuid_value(uuid_value)
    {}

    ALWAYS_INLINE constexpr UUID& operator=(const UUID& other)
    {
        m_uuid_value = other.m_uuid_value;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE constexpr bool is_valid() const { return (m_uuid_value != 0); }
    NODISCARD ALWAYS_INLINE constexpr u64 value() const { return m_uuid_value; }

    NODISCARD ALWAYS_INLINE constexpr bool operator==(const UUID& other) const { return (m_uuid_value == other.m_uuid_value); }
    NODISCARD ALWAYS_INLINE constexpr bool operator!=(const UUID& other) const { return (m_uuid_value != other.m_uuid_value); }

private:
    u64 m_uuid_value;
};

} // namespace SE
