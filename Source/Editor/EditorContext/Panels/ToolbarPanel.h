/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>

namespace SE
{

// Forward declarations.
struct EditorCameraController;

class ToolbarPanel
{
public:
    bool initialize();
    void shutdown();

    void on_update(float delta_time);
    void on_render_imgui();

public:
    NODISCARD ALWAYS_INLINE EditorCameraController* get_editor_camera_controller_context() const { return m_editor_camera_controller_context; }
    NODISCARD ALWAYS_INLINE bool has_editor_camera_controller_context() const { return (m_editor_camera_controller_context != nullptr); }
    void set_editor_camera_controller_context(EditorCameraController* editor_camera_controller_context);
    void clear_editor_camera_controller_context();

private:
    EditorCameraController* m_editor_camera_controller_context;
};

} // namespace SE
