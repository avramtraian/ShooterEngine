/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Assertions.h>
#include <Core/Platform/Platform.h>

namespace SE
{

class Timer
{
public:
    ALWAYS_INLINE Timer()
        : m_performance_counter_end(0)
    {
        m_performance_counter_start = Platform::get_current_tick_counter();
    }

    ALWAYS_INLINE void stop()
    {
        // The timer was already stopped.
        SE_ASSERT(!is_stopped());
        m_performance_counter_end = Platform::get_current_tick_counter();
    }

    NODISCARD ALWAYS_INLINE bool is_stopped() const { return (m_performance_counter_end > 0); }

public:
    //
    // Returns the number of ticks between the timer creation and the moment the `stop()` function was invoked.
    // If the timer hasn't been stopped yet, this function returns zero (0).
    //
    NODISCARD ALWAYS_INLINE u64 elapsed_ticks() const
    {
        if (!is_stopped())
        {
            // The timer hasn't been stopped yet.
            return 0;
        }

        SE_ASSERT(m_performance_counter_end >= m_performance_counter_start);
        const u64 elapsed = m_performance_counter_end - m_performance_counter_start;
        return elapsed;
    }

    //
    // Returns the number of seconds between the timer creation and the moment the `stop()` function was invoked.
    // If the timer hasn't been stopped yet, this function will return zero (0.0F).
    //
    NODISCARD ALWAYS_INLINE float elapsed_seconds() const
    {
        const u64 tick_count = elapsed_ticks();
        const u64 tick_frequency = Platform::get_tick_counter_frequency();
        return static_cast<float>(tick_count) / static_cast<float>(tick_frequency);
    }

    NODISCARD ALWAYS_INLINE float stop_and_get_elapsed_seconds()
    {
        stop();
        return elapsed_seconds();
    }

private:
    u64 m_performance_counter_start;
    u64 m_performance_counter_end;
};

} // namespace SE
