/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/OwnPtr.h"
#include "Core/Containers/Vector.h"
#include "Engine/Engine.h"

namespace SE {

class EditorEngine final : public Engine {
public:
    EditorEngine() = default;
    virtual ~EditorEngine() override = default;

    virtual bool initialize() override;
    virtual void shutdown() override;

    virtual void tick() override;

    virtual Window* create_window() override;
    virtual Window* find_window_by_native_handle(void* native_handle) override;

private:
    // The first element in this vector is always the primary window.
    Vector<OwnPtr<Window>> m_window_stack;
};

// It points to the same object as the 'g_engine' global variable.
// This pointer is initialized when the editor engine is instantiated.
extern EditorEngine* g_editor_engine;

} // namespace SE