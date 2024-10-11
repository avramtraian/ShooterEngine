--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

invalid_value = "<Invalid>"

targets = {}
targets.editor = "Editor"
targets.game = "Game"

function create_default_config_vars_object()
    config_vars_object = {}
    config_vars_object.target = targets.invalid
    config_vars_object.engine_root = invalid_value
    return config_vars_object
end

function begin_filter_configuration_editor()
    if config_vars.target == targets.game then
        filter "configurations:EditorDebug or EditorDevelopment"
    end
end

function begin_filter_configuration_game()
    if config_vars.target == targets.editor then
        filter "configurations:GameDebug or GameDevelopment or GameShipping"
    else
        print("WARN: 'begin_filter_configuration_game' called when 'config_vars.target' != 'targets.editor'")
    end
end

function configure_default_settings()
    filter "platforms:Windows"
        systemversion "latest"
        defines { "SE_PLATFORM_WINDOWS=1" }
    filter {}

    language "C++"
    cppdialect "C++20"

    rtti "Off"
    exceptionhandling "Off"
    characterset "Unicode"
    flags { "MultiProcessorCompile" }

    targetdir "%{wks.location}/Binaries/%{cfg.buildcfg}"
    objdir "%{wks.location}/Intermediate/Build"

    if config_vars.target == targets.editor then
        defines { "SE_CONFIGURATION_TARGET_EDITOR=1" }

        filter "configurations:Debug"
            optimize "off"
            symbols "on"
            defines { "SE_CONFIGURATION_DEBUG=1" }
        filter {}
        filter "configurations:Development"
            optimize "on"
            symbols "on"
            defines { "SE_CONFIGURATION_DEVELOPMENT=1" }
        filter {}
        filter "configurations:Shipping"
            optimize "speed"
            symbols "off"
            defines { "SE_CONFIGURATION_SHIPPING=1" }
        filter {}
    elseif config_vars.target == targets.game then
        begin_filter_configuration_editor()
            defines { "SE_CONFIGURATION_TARGET_EDITOR=1" }
        filter {}

        begin_filter_configuration_game()
            defines { "SE_CONFIGURATION_TARGET_GAME=1" }
        filter {}

        filter "configurations:GameDebug or EditorDebug"
            optimize "off"
            symbols "on"
            defines { "SE_CONFIGURATION_DEBUG=1" }
        filter {}
        filter "configurations:GameDevelopment or GameDevelopment"
            optimize "on"
            symbols "on"
            defines { "SE_CONFIGURATION_DEVELOPMENT=1" }
        filter {}
        filter "configurations:GameShipping"
            optimize "speed"
            symbols "off"
            defines { "SE_CONFIGURATION_SHIPPING=1" }
        filter {}
    end
end
