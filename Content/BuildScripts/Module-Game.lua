--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

-- Sanity check. Should never fail.
if not config_vars.target == targets.game then
    print("ERROR: Trying to configure the game project when the target is set to editor!")
    return
end

local function configure_as_editor()
    kind "SharedLib"
    
end

local function configure_as_game()
    kind "ConsoleApp"
end

project (config_vars.game_name)
    location (config_vars.game_root_directory.."/Source")
    configure_default_settings()

    -- Link against the engine binaries.
    links { (config_vars.engine_root.."/Binaries/%{cfg.buildcfg}/SE-Engine") }

    begin_filter_configuration_editor()
        kind "SharedLib"
    filter {}

    begin_filter_configuration_game()
        kind "ConsoleApp"
    filter {}

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
