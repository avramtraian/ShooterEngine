// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Input.h>
#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/Math/Vector.h>
#include <Runtime/Core/Platform/PlatformCoreInclude.h>

namespace SE
{

enum SwitchState : uint8_t
{
    Up,
    Down,
    PressedThisFrame,
    ReleasedThisFrame,
};

struct WindowsInputData
{
    HashMap<KeyCode, SwitchState> KeyStates;
    HashMap<MouseButton, SwitchState> MouseButtonStates;

    Optional<Vector2i> LastFrameMousePosition;
    Optional<Vector2i> CurrentFrameMousePosition;
    float MouseWheelScrollOffset = 0.0F;
};

static WindowsInputData* s_InputData;

bool Input::Initialize()
{
    if (s_InputData)
        return false;
    s_InputData = new WindowsInputData();

    // NOTE(Traian): While it is more efficient to query the key-state when the value is actually requested by the
    // user (by calling 'IsKeyDown' for example), to ensure that for the duration of the entire frame the input system
    // returns the same result when asked about the state of a key we create these tables and update them once per frame.

    for (uint16_t keyCodeValue = 1; keyCodeValue < (uint16_t)KeyCode::MaxEnumValue; ++keyCodeValue)
    {
        const KeyCode keyCode = (KeyCode)keyCodeValue;
        s_InputData->KeyStates.Add(keyCode, SwitchState::Up);
    }
    for (uint16_t mouseButtonValue = 1; mouseButtonValue < (uint8_t)MouseButton::MaxEnumValue; mouseButtonValue++)
    {
        const MouseButton mouseButton = (MouseButton)mouseButtonValue;
        s_InputData->MouseButtonStates.Add(mouseButton, SwitchState::Up);
    }

    return true;
}
    
void Input::Shutdown()
{
    if (!s_InputData)
        return;

    s_InputData->KeyStates.ClearAndShrink();
    s_InputData->MouseButtonStates.ClearAndShrink();

    delete s_InputData;
    s_InputData = nullptr;
}

void Input::AddSourceWindow(RefPtr<Window> window)
{
    if (window.IsValid())
    {
        window->GetOnMouseWheelScrolledDelegate().AddRaw(Input::HandleOnMouseWheelScrolled);
    }
}

static int TranslateKeyCodeToVirtualKey(KeyCode keyCode)
{
    // NOTE(Traian): The key-codes are translated to the Windows layer using the following documenation.
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

    const uint16_t keyCodeValue = (uint16_t)keyCode;

    // Check for alphabetical key-codes.
    if ((uint16_t)KeyCode::A <= keyCodeValue && keyCodeValue <= (uint16_t)KeyCode::Z)
        return 'A' + (keyCodeValue - (uint16_t)KeyCode::A);

    // Check for numerical key-codes.
    if ((uint16_t)KeyCode::Zero <= keyCodeValue && keyCodeValue <= (uint16_t)KeyCode::Nine)
        return '0' + (keyCodeValue - (uint16_t)KeyCode::Zero);

    // Check for function key-codes.
    if ((uint16_t)KeyCode::F1 <= keyCodeValue && keyCodeValue <= (uint16_t)KeyCode::F12)
        return VK_F1 + (keyCodeValue - (uint16_t)KeyCode::F1);

    switch (keyCode)
    {
        case KeyCode::Control:      return VK_CONTROL;
        case KeyCode::Shift:        return VK_SHIFT;
        case KeyCode::Alt:          return VK_MENU;
        case KeyCode::RightControl: return VK_RCONTROL;
        case KeyCode::RightShift:   return VK_RSHIFT;
        case KeyCode::RightAlt:     return VK_RMENU;
        case KeyCode::LeftControl:  return VK_LCONTROL;
        case KeyCode::LeftShift:    return VK_LSHIFT;
        case KeyCode::LeftAlt:      return VK_LMENU;
        case KeyCode::ArrowLeft:    return VK_LEFT;
        case KeyCode::ArrowRight:   return VK_RIGHT;
        case KeyCode::ArrowUp:      return VK_UP;
        case KeyCode::ArrowDown:    return VK_DOWN;
    }

    SE_ASSERT(!"Invalid key code!");
    return 0;
}

static int TranslateMouseButtonToVirtualKey(MouseButton mouseButton)
{
    // NOTE(Traian): The mouse-buttons are translated to the Windows layer using the following documenation.
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

    switch (mouseButton)
    {
        case MouseButton::Left:   return VK_LBUTTON;
        case MouseButton::Middle: return VK_MBUTTON;
        case MouseButton::Right:  return VK_RBUTTON;
    }

    SE_ASSERT(!"Invalid mouse button!");
    return 0;
}

static bool CheckIfKeyIsDown(KeyCode keyCode)
{
    const SHORT keyState = GetKeyState(TranslateKeyCodeToVirtualKey(keyCode));
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate
    // NOTE(Traian): The key is pressed if the "high-order bit" is set.
    return ((keyState & (1 << 15)) != 0);
}

static bool CheckIfMouseButtonIsDown(MouseButton mouseButton)
{
    const SHORT keyState = GetKeyState(TranslateMouseButtonToVirtualKey(mouseButton));
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate
    // NOTE(Traian): The key is pressed if the "high-order bit" is set.
    return ((keyState & (1 << 15)) != 0);
}

void Input::OnPreUpdate(float deltaTime)
{}

void Input::OnUpdate(float deltaTime)
{
    if (!s_InputData)
        return;

    // Update the key states.
    for (uint16_t keyCodeValue = 1; keyCodeValue < (uint16_t)KeyCode::MaxEnumValue; ++keyCodeValue)
    {
        const KeyCode keyCode = (KeyCode)keyCodeValue;
        SE_ASSERT(s_InputData->KeyStates.Contains(keyCode));
        const bool isDown = CheckIfKeyIsDown(keyCode);

        if (s_InputData->KeyStates[keyCode] == SwitchState::Up && isDown)
            s_InputData->KeyStates[keyCode] = SwitchState::PressedThisFrame;
        else if (s_InputData->KeyStates[keyCode] == SwitchState::Down && !isDown)
            s_InputData->KeyStates[keyCode] = SwitchState::ReleasedThisFrame;
        else
            s_InputData->KeyStates[keyCode] = isDown ? SwitchState::Down : SwitchState::Up;
    }

    // Update the mouse button states.
    for (uint16_t mouseButtonValue = 1; mouseButtonValue < (uint8_t)MouseButton::MaxEnumValue; mouseButtonValue++)
    {
        const MouseButton mouseButton = (MouseButton)mouseButtonValue;
        SE_ASSERT(s_InputData->MouseButtonStates.Contains(mouseButton));
        const bool isDown = CheckIfMouseButtonIsDown(mouseButton);

        if (s_InputData->MouseButtonStates[mouseButton] == SwitchState::Up && isDown)
            s_InputData->MouseButtonStates[mouseButton] = SwitchState::PressedThisFrame;
        else if (s_InputData->MouseButtonStates[mouseButton] == SwitchState::Down && !isDown)
            s_InputData->MouseButtonStates[mouseButton] = SwitchState::ReleasedThisFrame;
        else
            s_InputData->MouseButtonStates[mouseButton] = isDown ? SwitchState::Down : SwitchState::Up;
    }

    // Update the mouse position.
    s_InputData->LastFrameMousePosition = s_InputData->CurrentFrameMousePosition;
    POINT mousePosition = {};
    if (GetCursorPos(&mousePosition))
        s_InputData->CurrentFrameMousePosition = Vector2i(mousePosition.x, mousePosition.y);
}

void Input::OnPostUpdate(float deltaTime)
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return;

    s_InputData->MouseWheelScrollOffset = 0.0F;
}

bool Input::IsKeyDown(KeyCode keyCode)
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->KeyStates.Contains(keyCode));
    const SwitchState switchState = s_InputData->KeyStates.At(keyCode);
    return switchState == SwitchState::Down || switchState == SwitchState::PressedThisFrame;
}

bool Input::IsMouseButtonDown(MouseButton mouseButton)
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->MouseButtonStates.Contains(mouseButton));
    const SwitchState switchState = s_InputData->MouseButtonStates.At(mouseButton);
    return switchState == SwitchState::Down || switchState == SwitchState::PressedThisFrame;
}

bool Input::WasKeyPressedThisFrame(KeyCode keyCode)
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->KeyStates.Contains(keyCode));
    const SwitchState switchState = s_InputData->KeyStates.At(keyCode);
    return switchState == SwitchState::PressedThisFrame;
}

bool Input::WasKeyReleasedThisFrame(KeyCode keyCode)
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->KeyStates.Contains(keyCode));
    const SwitchState switchState = s_InputData->KeyStates.At(keyCode);
    return switchState == SwitchState::ReleasedThisFrame;
}

bool Input::WasMouseButtonPressedThisFrame(MouseButton mouseButton)
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->MouseButtonStates.Contains(mouseButton));
    const SwitchState switchState = s_InputData->MouseButtonStates.At(mouseButton);
    return switchState == SwitchState::PressedThisFrame;
}

bool Input::WasMouseButtonReleasedThisFrame(MouseButton mouseButton)
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->MouseButtonStates.Contains(mouseButton));
    const SwitchState switchState = s_InputData->MouseButtonStates.At(mouseButton);
    return switchState == SwitchState::ReleasedThisFrame;
}

int32_t Input::GetMouseDeltaX()
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return 0;

    // Check that the current mouse position and last mouse position optionals have values.
    if (!s_InputData->CurrentFrameMousePosition.HasValue() || !s_InputData->LastFrameMousePosition.HasValue())
        return 0;

    // Calculate the mouse delta position on the X-axis.
    return s_InputData->CurrentFrameMousePosition->X - s_InputData->LastFrameMousePosition->X;
}

int32_t Input::GetMouseDeltaY()
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return 0;

    // Check that the current mouse position and last mouse position optionals have values.
    if (!s_InputData->CurrentFrameMousePosition.HasValue() || !s_InputData->LastFrameMousePosition.HasValue())
        return 0;

    // Calculate the mouse delta position on the X-axis.
    return s_InputData->CurrentFrameMousePosition->Y - s_InputData->LastFrameMousePosition->Y;
}

float Input::GetMouseWheelScrollOffset()
{
    // Check that the input system has been initialized.
    if (!s_InputData)
        return 0.0F;

    return s_InputData->MouseWheelScrollOffset;
}

void Input::HandleOnMouseWheelScrolled(RefPtr<Window> sourceWindow, float scrollOffset)
{
    // Check that the input system has been initialized and if the source window is valid.
    if (!s_InputData || !sourceWindow.IsValid())
        return;

    // Accumulate the scroll offset.
    s_InputData->MouseWheelScrollOffset += scrollOffset;
}

}
