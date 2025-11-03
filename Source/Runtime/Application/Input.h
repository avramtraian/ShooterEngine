// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>

namespace SE
{

enum class KeyCode : uint16
{
    // clang-format off
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
    // clang-format on
};

enum class MouseButton : uint8
{
    // clang-format off
    Unknown = 0,
    Left, Middle, Right,
    MaxEnumValue,
    // clang-format on
};

class Input
{
    SE_MAKE_NAMESPACE_CLASS(Input);

public:
    RUNTIME_API static bool Initialize();
    RUNTIME_API static void Shutdown();

    // NOTE(Traian): The input system tries to not depend on events processed by the provided source windows, and instead
    // determine the state of the keys or the mouse position globally (potential security risk?). However, depending on the
    // platform, certain input variables can't be queried globally (such as the mouse wheel scroll offset on Windows), and
    // thus all windows that receive input events should be registered using this function.
    RUNTIME_API static void AddSourceWindow(RefPtr<Window> window);

    RUNTIME_API static void OnPreUpdate(float deltaTime);
    RUNTIME_API static void OnUpdate(float deltaTime);
    RUNTIME_API static void OnPostUpdate(float deltaTime);

public:
    RUNTIME_API static bool IsKeyDown(KeyCode keyCode);
    RUNTIME_API static bool IsMouseButtonDown(MouseButton mouseButton);

    RUNTIME_API static bool WasKeyPressedThisFrame(KeyCode keyCode);
    RUNTIME_API static bool WasKeyReleasedThisFrame(KeyCode keyCode);

    RUNTIME_API static bool WasMouseButtonPressedThisFrame(MouseButton mouseButton);
    RUNTIME_API static bool WasMouseButtonReleasedThisFrame(MouseButton mouseButton);

    RUNTIME_API static int32 GetMouseDeltaX();
    RUNTIME_API static int32 GetMouseDeltaY();
    RUNTIME_API static float GetMouseWheelScrollOffset();

private:
    static void HandleOnMouseWheelScrolled(RefPtr<Window> sourceWindow, float scrollOffset);
};

} // namespace SE
