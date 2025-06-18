// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Application/Input.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/Math/Vector.h>
#include <Runtime/Core/Platform/PlatformCoreInclude.h>

#include <unordered_map>

namespace SE
{

enum KeyState : uint8_t
{
    Up,
    Down,
    PressedThisFrame,
    ReleasedThisFrame,
};

struct SourceWindow
{
    Window* Window { nullptr };
    Vector<WindowEventCallbackID> CallbackIDs;
};

struct WindowsInputData
{
    std::unordered_map<KeyCode, KeyState> KeyStates;
    std::unordered_map<MouseButton, KeyState> MouseButtonStates;
    Vector<SourceWindow> SourceWindows;

    Vector2i LastFrameMousePosition;
    Vector2i CurrentFrameMousePosition;
    float MouseWheelScrollOffset = 0.0F;
};

static WindowsInputData* s_InputData;

bool Input::Initialize(const InputInfo& info)
{
    if (s_InputData)
        return false;
    s_InputData = new WindowsInputData();

    for (Window* window : info.SourceWindows)
    {
        const WindowEventCallbackID callbackID = window->AddEventCallback(EventType::MouseWheelScrolled,
            [](const Window&, const Event& opaqueEvent)
            {
                const MouseWheelScrolledEvent& event = (const MouseWheelScrolledEvent&)opaqueEvent;
                s_InputData->MouseWheelScrollOffset = event.GetScrollOffset();
            }
        );

        SourceWindow sourceWindow = {};
        sourceWindow.Window = window;
        sourceWindow.CallbackIDs.Add(callbackID);
        s_InputData->SourceWindows.Add(Move(sourceWindow));
    }

    // NOTE(Traian): While it is more efficient to query the key-state when the value is actually requested by the
    // user (by calling 'IsKeyDown' for example), to ensure that for the duration of the entire frame the input system
    // returns the same result when asked about the state of a key we create these tables and update them once per frame.

    for (uint16_t keyCodeValue = 1; keyCodeValue < (uint16_t)KeyCode::MaxEnumValue; ++keyCodeValue)
    {
        const KeyCode keyCode = (KeyCode)keyCodeValue;
        s_InputData->KeyStates.insert({ keyCode, KeyState::Up });
    }
    for (uint16_t mouseButtonValue = 1; mouseButtonValue < (uint8_t)MouseButton::MaxEnumValue; mouseButtonValue++)
    {
        const MouseButton mouseButton = (MouseButton)mouseButtonValue;
        s_InputData->MouseButtonStates.insert({ mouseButton, KeyState::Up });
    }

    return true;
}
    
void Input::Shutdown()
{
    if (!s_InputData)
        return;

    /* Remove callbacks from the source windows. */
    for (const SourceWindow& sourceWindow : s_InputData->SourceWindows)
    {
        for (WindowEventCallbackID callbackID : sourceWindow.CallbackIDs)
            sourceWindow.Window->RemoveEventCallback(callbackID);
    }

    s_InputData->SourceWindows.Clear();
    s_InputData->KeyStates.clear();
    s_InputData->MouseButtonStates.clear();

    delete s_InputData;
    s_InputData = nullptr;
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

void Input::OnUpdate()
{
    if (!s_InputData)
        return;

    // Update key states for key-codes and mouse-buttons.
    for (uint16_t keyCodeValue = 1; keyCodeValue < (uint16_t)KeyCode::MaxEnumValue; ++keyCodeValue)
    {
        const KeyCode keyCode = (KeyCode)keyCodeValue;
        SE_ASSERT(s_InputData->KeyStates.contains(keyCode));
        const bool isDown = CheckIfKeyIsDown(keyCode);

        if (s_InputData->KeyStates[keyCode] == KeyState::Up && isDown)
            s_InputData->KeyStates[keyCode] = KeyState::PressedThisFrame;
        else if (s_InputData->KeyStates[keyCode] == KeyState::Down && !isDown)
            s_InputData->KeyStates[keyCode] = KeyState::ReleasedThisFrame;
        else
            s_InputData->KeyStates[keyCode] = isDown ? KeyState::Down : KeyState::Up;
    }
    for (uint16_t mouseButtonValue = 1; mouseButtonValue < (uint8_t)MouseButton::MaxEnumValue; mouseButtonValue++)
    {
        const MouseButton mouseButton = (MouseButton)mouseButtonValue;
        SE_ASSERT(s_InputData->MouseButtonStates.contains(mouseButton));
        const bool isDown = CheckIfMouseButtonIsDown(mouseButton);

        if (s_InputData->MouseButtonStates[mouseButton] == KeyState::Up && isDown)
            s_InputData->MouseButtonStates[mouseButton] = KeyState::PressedThisFrame;
        else if (s_InputData->MouseButtonStates[mouseButton] == KeyState::Down && !isDown)
            s_InputData->MouseButtonStates[mouseButton] = KeyState::ReleasedThisFrame;
        else
            s_InputData->MouseButtonStates[mouseButton] = isDown ? KeyState::Down : KeyState::Up;
    }

    // Update the mouse position.
    s_InputData->LastFrameMousePosition = s_InputData->CurrentFrameMousePosition;
    POINT mousePosition = {};
    if (GetCursorPos(&mousePosition))
        s_InputData->CurrentFrameMousePosition = Vector2i(mousePosition.x, mousePosition.y);
}

void Input::OnPostUpdate()
{
    if (!s_InputData)
        return;
    s_InputData->MouseWheelScrollOffset = 0.0F;
}

bool Input::IsKeyDown(KeyCode keyCode)
{
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->KeyStates.contains(keyCode));
    const KeyState keyState = s_InputData->KeyStates.at(keyCode);
    return keyState == KeyState::Down || keyState == KeyState::PressedThisFrame;
}

bool Input::IsMouseButtonDown(MouseButton mouseButton)
{
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->MouseButtonStates.contains(mouseButton));
    const KeyState keyState = s_InputData->MouseButtonStates.at(mouseButton);
    return keyState == KeyState::Down || keyState == KeyState::PressedThisFrame;
}

bool Input::WasKeyPressedThisFrame(KeyCode keyCode)
{
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->KeyStates.contains(keyCode));
    const KeyState keyState = s_InputData->KeyStates.at(keyCode);
    return keyState == KeyState::PressedThisFrame;
}

bool Input::WasKeyReleasedThisFrame(KeyCode keyCode)
{
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->KeyStates.contains(keyCode));
    const KeyState keyState = s_InputData->KeyStates.at(keyCode);
    return keyState == KeyState::ReleasedThisFrame;
}

bool Input::WasMouseButtonPressedThisFrame(MouseButton mouseButton)
{
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->MouseButtonStates.contains(mouseButton));
    const KeyState keyState = s_InputData->MouseButtonStates.at(mouseButton);
    return keyState == KeyState::PressedThisFrame;
}

bool Input::WasMouseButtonReleasedThisFrame(MouseButton mouseButton)
{
    if (!s_InputData)
        return false;

    SE_ASSERT(s_InputData->MouseButtonStates.contains(mouseButton));
    const KeyState keyState = s_InputData->MouseButtonStates.at(mouseButton);
    return keyState == KeyState::ReleasedThisFrame;
}

int32_t Input::GetMousePositionX()
{
    if (!s_InputData)
        return 0;
    return s_InputData->CurrentFrameMousePosition.X;
}

int32_t Input::GetMousePositionY()
{
    if (!s_InputData)
        return 0;
    return s_InputData->CurrentFrameMousePosition.Y;
}

int32_t Input::GetMouseDeltaX()
{
    if (!s_InputData)
        return 0;
    return s_InputData->CurrentFrameMousePosition.X - s_InputData->LastFrameMousePosition.X;
}

int32_t Input::GetMouseDeltaY()
{
    if (!s_InputData)
        return 0;
    return s_InputData->CurrentFrameMousePosition.Y - s_InputData->LastFrameMousePosition.Y;
}

float Input::GetMouseWheelScrollOffset()
{
    if (!s_InputData)
        return 0.0F;
    return s_InputData->MouseWheelScrollOffset;
}

}
