// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Application/Window.h>
#include <Runtime/Core/Platform/PlatformCoreInclude.h>

namespace SE
{

class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowInfo& info);
    virtual ~WindowsWindow() override;

    NODISCARD virtual uint32 GetSizeX() const override;
    NODISCARD virtual uint32 GetSizeY() const override;
    NODISCARD virtual WindowMode GetCurrentMode() const override;

    NODISCARD ALWAYS_INLINE virtual void* GetNativeHandle() const override { return m_WindowHandle; }
    NODISCARD ALWAYS_INLINE virtual bool IsRequestedToClose() const override { return m_IsRequestedToClose; }

    NODISCARD ALWAYS_INLINE virtual OnWindowResizedDelegate& GetOnWindowResizedDelegate() override { return m_OnWindowResizedDelegate; }
    NODISCARD ALWAYS_INLINE virtual OnWindowClosedDelegate& GetOnWindowClosedDelegate() override { return m_OnWindowClosedDelegate; }

    NODISCARD ALWAYS_INLINE virtual OnMouseWheelScrolledDelegate& GetOnMouseWheelScrolledDelegate() override { return m_OnMouseWheelScrolledDelegate; }

public:
    virtual void SetSize(Optional<uint32> sizeX, Optional<uint32> sizeY) override;
    virtual void SetCurrentMode(WindowMode mode) override;

    virtual void ProcessEventQueue() override;
    virtual void Close() override;

private:
    void EnterFullscreen(WindowMode previousMode);
    void ExitFullscreen(WindowMode newMode);

private:
    HWND m_WindowHandle;
    bool m_IsRequestedToClose;

    OnWindowResizedDelegate m_OnWindowResizedDelegate;
    OnWindowClosedDelegate m_OnWindowClosedDelegate;
    
    OnMouseWheelScrolledDelegate m_OnMouseWheelScrolledDelegate;

    struct FullscreenState
    {
        WINDOWPLACEMENT PreviousWindowPlacement;
        WindowMode PreviousMode;
    };
    Optional<FullscreenState> m_FullscreenState;
};

}
