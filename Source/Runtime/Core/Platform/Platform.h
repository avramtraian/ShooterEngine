/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/StringView.h>
#include <Core/CoreTypes.h>

namespace SE
{

class Platform
{
public:
    enum class ConsoleColor : u8
    {
        Blue,
        Green,
        Red,
        LightBlue,
        LightGreen,
        LightRed,
        Aqua,
        Yellow,
        Magenta,
        LightAqua,
        LightYellow,
        LightMagenta,
        Black,
        Gray,
        DarkGray,
        White,
    };

public:
    SHOOTER_API static bool initialize();
    SHOOTER_API static void shutdown();

    SHOOTER_API static u64 get_current_tick_counter();
    SHOOTER_API static u64 get_tick_counter_frequency();

    SHOOTER_API static void write_to_console(StringView message, ConsoleColor text_color, ConsoleColor background_color);
};

} // namespace SE
