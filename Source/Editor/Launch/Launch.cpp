/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Containers/Span.h>
#include <Core/FileSystem/FileSystem.h>
#include <Core/Platform/Platform.h>
#include <EditorEngine.h>

namespace SE
{

//
// The non-native entry point of the editor. It is directly invoked by the native entry point
// and represents the entire lifetime of the application.
//
// Unlike the 'main' function, the first command line argument passed to this function is
// not the path of the executable.
//
static i32 guarded_main(Span<char*> command_line_arguments)
{
    if (!Platform::initialize())
        return 1;

    // Set the engine root as the process working directory.
    FileSystem::set_working_directory("../../"sv);

    // Initialize the engine.
    Engine::instantiate<EditorEngine>();
    if (!g_engine->initialize())
    {
        // TODO: Log error message.
        return 1;
    }

    // Entry the game loop.
    while (g_engine->is_running())
        g_engine->update();

    // Shutdown the engine.
    g_engine->shutdown();
    Engine::destroy();

    Platform::shutdown();
    return 0;
}

} // namespace SE

int main(int argument_count, char** arguments)
{
    SE::Span<char*> command_line_arguments(arguments + 1, argument_count - 1);
    SE::i32 return_code = SE::guarded_main(command_line_arguments);
    return static_cast<int>(return_code);
}
