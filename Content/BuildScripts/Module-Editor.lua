--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

-- Sanity check. Should never fail.
if not config_vars.target == targets.editor then
    print("ERROR: Trying to configure the editor project when the target is set to game!")
    return
end

project "SE-Editor"
    location "%{wks.location}/Source/Editor"
    kind "ConsoleApp"
    configure_default_settings()

    begin_filter_configuration_editor()
        kind "ConsoleApp"
    end_filter()

    begin_filter_configuration_game()
        kind "None"
    end_filter()

    -- Always set the working directory where the executable is located on disk.
    debugdir "%{wks.location}/Binaries/%{cfg.buildcfg}"

    links
    {
        "SE-Engine",
        "SE-ImGui",
        "SE-YamlCPP"
    }

    files
    {
        "%{wks.location}/Source/Editor/**.cpp",
        "%{wks.location}/Source/Editor/**.h",
        "%{wks.location}/Source/Editor/**.inl"
    }

    includedirs
    {
        "%{wks.location}/Source/Editor",
        "%{wks.location}/Source/Runtime",
        "%{wks.location}/Source",

        -- Third party includes.
        "%{wks.location}/Source/ThirdParty/yaml-cpp/include",
        "%{wks.location}/Source/ThirdParty/ImGui",
    }

    defines { "YAML_CPP_STATIC_DEFINE" }
-- endproject "SE-Editor"
