// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Engine/Engine.h>
#include <Editor/EditorEngine/EditorEngine.h>

namespace SE
{

static int32 EditorGuardedMain()
{
    // Initialize the editor engine.
    GEditorEngine = new EditorEngine();
    g_Engine = GEditorEngine;

    if (!g_Engine->Initialize())
    {
        // Failed to initialize the engine.
        return 1;
    }

    // Run the main editor loop.
    g_Engine->Execute();

    // Shutdown the editor engine.
    g_Engine->Shutdown();
    delete GEditorEngine;
    GEditorEngine = nullptr;
    g_Engine = nullptr;

    return 0;
}

}

int main()
{
    const int returnCode = SE::EditorGuardedMain();
    return returnCode;
}
