/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/Platform/Platform.h"
#include "Core/Platform/Windows/WindowsHeaders.h"

namespace SE {

struct WindowsPlatformData {
    HANDLE console_handle;
    Platform::ConsoleColor console_text_color;
    Platform::ConsoleColor console_background_color;
};

static WindowsPlatformData* s_windows_platform = nullptr;

bool Platform::initialize()
{
    if (s_windows_platform)
        return false;

    s_windows_platform = new WindowsPlatformData();
    s_windows_platform->console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (s_windows_platform->console_handle == INVALID_HANDLE_VALUE)
        return false;

    return true;
}

void Platform::shutdown()
{
    if (!s_windows_platform)
        return;

    // Set the console to the default color scheme.
    SetConsoleTextAttribute(s_windows_platform->console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    CloseHandle(s_windows_platform->console_handle);
    s_windows_platform->console_handle = INVALID_HANDLE_VALUE;

    delete s_windows_platform;
    s_windows_platform = nullptr;
}

static WORD get_console_foreground_color(Platform::ConsoleColor color)
{
    switch (color) {
        case Platform::ConsoleColor::Blue: return FOREGROUND_BLUE;
        case Platform::ConsoleColor::Green: return FOREGROUND_GREEN;
        case Platform::ConsoleColor::Red: return FOREGROUND_RED;
        case Platform::ConsoleColor::LightBlue: return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case Platform::ConsoleColor::LightGreen: return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case Platform::ConsoleColor::LightRed: return FOREGROUND_RED | FOREGROUND_INTENSITY;
        case Platform::ConsoleColor::Aqua: return FOREGROUND_BLUE | FOREGROUND_GREEN;
        case Platform::ConsoleColor::Yellow: return FOREGROUND_GREEN | FOREGROUND_RED;
        case Platform::ConsoleColor::Magenta: return FOREGROUND_RED | FOREGROUND_BLUE;
        case Platform::ConsoleColor::LightAqua: return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case Platform::ConsoleColor::LightYellow: return FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
        case Platform::ConsoleColor::LightMagenta: return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case Platform::ConsoleColor::Black: return 0;
        case Platform::ConsoleColor::Gray: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        case Platform::ConsoleColor::DarkGray: return FOREGROUND_INTENSITY;
        case Platform::ConsoleColor::White:
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    }

    SE_ASSERT(false);
    return 0;
}

static WORD get_console_background_color(Platform::ConsoleColor color)
{
    WORD foreground_color = get_console_foreground_color(color);
    return (foreground_color << 4);
}

void Platform::write_to_console(StringView message, ConsoleColor text_color, ConsoleColor background_color)
{
    SE_ASSERT(s_windows_platform);
    if (s_windows_platform->console_handle == INVALID_HANDLE_VALUE)
        return;

    WORD foreground = get_console_foreground_color(text_color);
    WORD background = get_console_background_color(background_color);

    if (s_windows_platform->console_text_color != text_color ||
        s_windows_platform->console_background_color != background_color) {
        SetConsoleTextAttribute(s_windows_platform->console_handle, foreground | background);
    }

    DWORD written_characters;
    // TODO: Convert the UTF-8 message string to UTF-16 instead of only allowing ASCII messages.
    WriteConsoleA(
        s_windows_platform->console_handle,
        message.byte_span().elements(),
        (DWORD)(message.byte_span().count()),
        &written_characters,
        NULL
    );
}

} // namespace SE