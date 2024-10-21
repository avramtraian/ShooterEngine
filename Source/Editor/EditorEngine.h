/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <EditorContext/EditorContext.h>
#include <Engine/Application/Events/Event.h>
#include <Engine/Engine.h>

namespace SE
{

class EditorEngine : public Engine
{
public:
    EditorEngine() = default;
    virtual ~EditorEngine() override = default;

    virtual bool initialize() override;
    virtual void shutdown() override;

    virtual void update() override;

public:
    NODISCARD ALWAYS_INLINE EditorContext& context() { return m_editor_context; }
    NODISCARD ALWAYS_INLINE const EditorContext& context() const { return m_editor_context; }

    virtual String get_engine_root_directory() const override;

private:
    static void on_event(const Event& in_event);

    void preload_project();
    void load_project();

private:
    float m_last_frame_delta_time { 0 };

    EditorContext m_editor_context;
};

// It points to the same object as the 'g_engine' global variable.
// This pointer is initialized when the editor engine is instantiated.
extern EditorEngine* g_editor_engine;

} // namespace SE
