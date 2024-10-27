/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/CoreTypes.h>

namespace SE
{

class UUID
{
public:
    NODISCARD ALWAYS_INLINE static constexpr UUID invalid() { return UUID(0); }

    NODISCARD SHOOTER_API static UUID create();

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

    ALWAYS_INLINE constexpr UUID(UUID&& other) noexcept
        : m_uuid_value(other.m_uuid_value)
    {
        other.m_uuid_value = 0;
    }

    ALWAYS_INLINE constexpr UUID& operator=(const UUID& other)
    {
        m_uuid_value = other.m_uuid_value;
        return *this;
    }

    ALWAYS_INLINE constexpr UUID& operator=(UUID&& other) noexcept
    {
        // Handle self-assignment case.
        if (this == &other)
            return *this;

        m_uuid_value = other.m_uuid_value;
        other.m_uuid_value = 0;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE constexpr u64 value() const { return m_uuid_value; }
    NODISCARD ALWAYS_INLINE constexpr bool is_valid() const { return (m_uuid_value != 0); }

public:
    NODISCARD ALWAYS_INLINE constexpr bool operator==(const UUID& other) const { return (m_uuid_value == other.m_uuid_value); }
    NODISCARD ALWAYS_INLINE constexpr bool operator!=(const UUID& other) const { return (m_uuid_value != other.m_uuid_value); }

private:
    u64 m_uuid_value;
};

} // namespace SE
