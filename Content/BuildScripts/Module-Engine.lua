--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

local function configure_as_target_editor()
    kind "SharedLib"
    location "%{wks.location}/Source/Runtime"

    defines
    {
        "SE_EXPORT_ENGINE_API"
    }
end

local function configure_as_target_game()
    location "%{wks.location}/Intermediate/ProjectFiles"

    begin_filter_configuration_editor()
        kind "None"
    filter {}

    begin_filter_configuration_game()
        kind "StaticLib"
    filter {}
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
