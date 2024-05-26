/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/OwnPtr.h"
#include "Core/Containers/Vector.h"
#include "Engine/Engine.h"

namespace SE {

class EditorEngine final : public Engine
{
public:
    EditorEngine() = default;
    virtual ~EditorEngine() override = default;

    virtual bool initialize() override;
    virtual void shutdown() override;

    virtual void tick() override;

    virtual Window* create_window() override;
    virtual Window* find_window_by_native_handle(void* native_handle) override;

public:
    virtual const String& get_engine_root_directory() const override { return m_engine_root_directory; }
    NODISCARD ALWAYS_INLINE const String& get_project_name() const { return m_project_name; }
    NODISCARD ALWAYS_INLINE const String& get_project_root_directory() const { return m_project_root_directory; }

    String get_project_file_filepath() const;
    String get_project_content_directory() const;
    String get_project_source_directory() const;

private:
    // The first element in this vector is always the primary window.
    Vector<OwnPtr<Window>> m_window_stack;

    String m_engine_root_directory;
    String m_project_name;
    String m_project_root_directory;
};

// It points to the same object as the 'g_engine' global variable.
// This pointer is initialized when the editor engine is instantiated.
extern EditorEngine* g_editor_engine;

} // namespace SE
