// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Engine/GameEngine.h>

namespace SE
{

static int32 guarded_main()
{
    g_game_engine = new GameEngine();
    g_engine = g_game_engine;

    if (!g_engine->initialize())
    {
        /* Failed to initialize the engine. */
        return 1;
    }

    g_engine->execute();
    g_engine->shutdown();

    delete g_game_engine;
    g_game_engine = nullptr;
    g_engine = nullptr;

    return 0;
}

}

int main()
{
    const int return_code = SE::guarded_main();
    return return_code;
}
