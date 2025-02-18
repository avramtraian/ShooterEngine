-- Copyright (c) 2024-2025 Traian Avram. All rights reserved.

-- Global variables.
g_binaries_directory = "%{wks.location}/Binaries/%{cfg.buildcfg}"
g_binaries_intermediate_directory = "%{wks.location}/Intermediate/Build"

-- Invokes the Premake 'filter' command with various arguments.
function begin_filter_target_editor()             filter "configurations:EditorDebug or EditorDevelopment"             end
function begin_filter_target_game()               filter "configurations:GameDebug or GameDevelopment or GameShipping" end
function begin_filter_configuration_debug()       filter "configurations:EditorDebug or GameDebug"                     end
function begin_filter_configuration_development() filter "configurations:EditorDevelopment or GameDevelopment"         end
function begin_filter_configuration_shipping()    filter "configurations:GameShipping"                                 end

-- Invokes the Premake 'filter' without any argument, esentially clearing the filtering list.
-- Wrapper around 'filter {}' - exists in order to provide begin/end function symmetry.
function end_filter() filter {} end

function default_project_configuration()
    language "C++"
    cppdialect "C++20"
    rtti "off"
    exceptionhandling "off"
    characterset "unicode"
    targetdir (g_binaries_directory)
    objdir (g_binaries_intermediate_directory)

    filter "platforms:Win64"
        defines { "SE_PLATFORM_WIN64=1" }
    filter {}

    filter "configurations:EditorDebug"
        defines { "SE_CONFIGURATION_EDITOR_DEBUG=1" }
    filter "configurations:EditorDevelopment"
        defines { "SE_CONFIGURATION_EDITOR_DEVELOPMENT=1" }
    filter "configurations:GameDebug"
        defines { "SE_CONFIGURATION_GAME_DEBUG=1" }
    filter "configurations:GameDevelopment"
        defines { "SE_CONFIGURATION_GAME_DEVELOPMENT=1" }
    filter "configurations:GameShipping"
        defines { "SE_CONFIGURATION_GAME_SHIPPING=1" }
    filter {}

    includedirs "%{wks.location}/Source"
end
