/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "EditorEngine.h"

namespace SE
{

EditorEngine* g_editor_engine = nullptr;

bool EditorEngine::initialize()
{
    if (!Engine::initialize())
        return false;

    g_editor_engine = this;
    return true;
}

void EditorEngine::shutdown()
{
    Engine::shutdown();
    g_editor_engine = nullptr;
}

void EditorEngine::tick()
{
    Engine::tick();
}

} // namespace SE