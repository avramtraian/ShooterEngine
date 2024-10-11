--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

local function configure_as_editor()
    kind "SharedLib"

    filter "configurations:EditorDebug"
        links { (config_vars.engine_root.."/Binaries/Debug/SE-Engine") }
    filter {}

    filter "configurations:EditorDevelopment"
        links { (config_vars.engine_root.."/Binaries/Development/SE-Engine") }
    filter {}
end

local function configure_as_game()
    kind "ConsoleApp"
end

project (config_vars.game_name)
    location (config_vars.game_root_directory.."/Source")
    configure_default_settings()

    begin_filter_configuration_editor()
        configure_as_editor()
    filter {}

    begin_filter_configuration_game()
        configure_as_game()
    filter {}

    links
    {
        "SE-Engine"
    }

    files
    {
        "%{wks.location}/Source/**.cpp",
        "%{wks.location}/Source/**.h",
        "%{wks.location}/Source/**.inl"
    }

    includedirs
    {
        "%{wks.location}/Source",
        (config_vars.engine_root.."/Source/Runtime")
    }
-- endproject (config_vars.game_name)
