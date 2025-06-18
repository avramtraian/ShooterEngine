// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Engine/GameEngine.h>

namespace SE
{

static int32 guarded_main()
{
    g_GameEngine = new GameEngine();
    g_Engine = g_GameEngine;

    if (!g_Engine->Initialize())
    {
        /* Failed to initialize the engine. */
        return 1;
    }

    g_Engine->Execute();
    g_Engine->Shutdown();

    delete g_GameEngine;
    g_GameEngine = nullptr;
    g_Engine = nullptr;

    return 0;
}

}

int main()
{
    const int return_code = SE::guarded_main();
    return return_code;
}
