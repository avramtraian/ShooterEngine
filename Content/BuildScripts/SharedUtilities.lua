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
    config_vars_object.target = invalid_value
    config_vars_object.engine_root = invalid_value
    config_vars_object.game_name = invalid_value
    config_vars_object.game_root_directory = invalid_value
    return config_vars_object
end

function begin_filter_configuration_editor()
    filter "configurations:EditorDebug or EditorDevelopment"
end

function begin_filter_configuration_game()
    filter "configurations:GameDebug or GameDevelopment or GameShipping"
end

function begin_filter_configuration_debug()
    filter "configurations:EditorDebug or GameDebug"
end

function begin_filter_configuration_development()
    filter "configurations:EditorDevelopment or GameDevelopment"
end

function end_filter()
    filter {}
end

function begin_filter_configuration_shipping()
    filter "configurations:GameShipping"
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

    begin_filter_configuration_editor()
        defines { "SE_CONFIGURATION_TARGET_EDITOR=1" }
    end_filter()

    begin_filter_configuration_game()
        defines { "SE_CONFIGURATION_TARGET_GAME=1" }
    end_filter()

    begin_filter_configuration_debug()
        optimize "off"
        symbols "on"
        defines { "SE_CONFIGURATION_DEBUG=1" }
    end_filter()

    begin_filter_configuration_development()
        optimize "on"
        symbols "on"
        defines { "SE_CONFIGURATION_DEVELOPMENT=1" }
    end_filter()

    begin_filter_configuration_shipping()
        optimize "speed"
        symbols "off"
        defines { "SE_CONFIGURATION_SHIPPING=1" }
    end_filter()
end
