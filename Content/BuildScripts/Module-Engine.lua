--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

local function configure_as_target_editor()
    location "%{wks.location}/Source/Runtime"
    
    begin_filter_configuration_editor()
        kind "SharedLib"
    end_filter()

    begin_filter_configuration_game()
        kind "StaticLib"
    end_filter()

    defines
    {
        "SE_EXPORT_ENGINE_API"
    }
end

local function configure_as_target_game()
    location "%{wks.location}/Intermediate/ProjectFiles"
    kind "None"
end

project "SE-Engine"
    configure_default_settings()

    if config_vars.target == targets.editor then
        configure_as_target_editor()
    elseif config_vars.target == targets.game then
        configure_as_target_game()
    end

    files
    {
        (config_vars.engine_root.."/Source/Runtime/**.cpp"),
        (config_vars.engine_root.."/Source/Runtime/**.h"),
        (config_vars.engine_root.."/Source/Runtime/**.inl")
    }

    includedirs
    {
        (config_vars.engine_root.."/Source/Runtime"),
    }

    filter "platforms:Windows"
        links
        {
            "d3d11.lib",
            "dxgi.lib",
            "d3dcompiler.lib"
        }
    filter {}
-- endproject "SE-Engine"
