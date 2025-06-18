// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Core/Containers/Vector.h>

namespace SE
{

enum class KeyCode : uint16_t
{
    Unknown = 0,

    // NOTE(Traian): The order in which the key codes appear inside each category is important, as
    // the function that translates these key-codes into platform virtual key-codes assume this exact
    // order. However, the order of the categories doesn't matter.

    // Alphabetical key-codes.
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Numerical key-codes.
    Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,

    // Modifiers key-codes.
    Shift, Control, Alt,
    LeftShift, LeftControl, LeftAlt,
    RightShift, RightControl, RightAlt,

    // Arrow key-codes.
    ArrowLeft, ArrowRight, ArrowUp, ArrowDown,

    // Function key-codes.
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    MaxEnumValue,
};

enum class MouseButton : uint8_t
{
    Unknown = 0,
    Left, Middle, Right,
    MaxEnumValue,
};

struct InputInfo
{
public:
    Vector<Window*> SourceWindows;

public:
    inline InputInfo& AddSourceWindow  (Window* window)                         { SourceWindows.Add(window);  return *this; }
    inline InputInfo& AddSourceWindows (std::initializer_list<Window*> windows) { SourceWindows.Add(windows); return *this; }
};

class Input
{
    SE_MAKE_NAMESPACE_CLASS(Input);

public:
    static bool Initialize(const InputInfo& inputInfo);
    static void Shutdown();
    static void OnUpdate();
    static void OnPostUpdate();

public:
    static bool IsKeyDown(KeyCode keyCode);
    static bool IsMouseButtonDown(MouseButton mouseButton);

    static bool WasKeyPressedThisFrame(KeyCode keyCode);
    static bool WasKeyReleasedThisFrame(KeyCode keyCode);

    static bool WasMouseButtonPressedThisFrame(MouseButton mouseButton);
    static bool WasMouseButtonReleasedThisFrame(MouseButton mouseButton);

    static int32_t GetMousePositionX();
    static int32_t GetMousePositionY();
    static int32_t GetMouseDeltaX();
    static int32_t GetMouseDeltaY();
    static float GetMouseWheelScrollOffset();
};

}
