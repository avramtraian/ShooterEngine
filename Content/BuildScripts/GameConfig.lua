--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

-- Utilities library.
require "SharedUtilities"

config_vars = create_default_config_vars_object()
config_vars.target = targets.game
config_vars.engine_root = os.getenv("ENGINE_ROOT");
config_vars.game_name = os.getenv("GAME_NAME")
config_vars.game_root_directory = os.getenv("GAME_ROOT_DIRECTORY")

if (config_vars.engine_root == nil) then
    print("The 'ENGINE_ROOT' environment variable is not set! Aborting.")
    return
end

if (config_vars.game_name == nil) then
    print("The 'GAME_NAME' environment variable is not set! Aborting.")
    return
end

if (config_vars.game_root_directory == nil) then
    print("The 'GAME_ROOT_DIRECTORY' environment variable is not set! Aborting.")
    return
end

workspace (config_vars.game_name)
    location (config_vars.game_root_directory)

    configurations
    {
        "EditorDebug",
        "EditorDevelopment",

        "GameDebug",
        "GameDevelopment",
        "GameShipping"
    }

    platforms
    {
        "Windows"
    }

    filter "platforms:Windows"
        system "Windows"
        architecture "x64"
    filter {}

    startproject (config_vars.game_name)

    group "Game"
        include (config_vars.engine_root.."/Content/BuildScripts/Module-Game")
    group "Engine"
        include (config_vars.engine_root.."/Content/BuildScripts/Module-Engine")
    group ""
-- endworkspace (config_vars.game_name)
