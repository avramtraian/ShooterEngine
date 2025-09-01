-- Copyright (c) 2024-2025 Traian Avram. All rights reserved.

project "SE-Editor"
    location "%{wks.location}/Source/Editor"
    default_project_configuration()
    defines { "SE_PROJECT_EDITOR" }

    begin_filter_target_editor()
        -- The editor is a standalone executable that links against the runtime binaries and dynamically loads
        -- the game binaries.
        kind "ConsoleApp"
    end_filter()

    begin_filter_target_game()
        -- The editor project itself should not be compiled when in a Game build configuration.
        kind "None"
    end_filter()

    files {
        "%{wks.location}/Source/Editor/**.cpp",
        "%{wks.location}/Source/Editor/**.cc",
        "%{wks.location}/Source/Editor/**.cxx",
        "%{wks.location}/Source/Editor/**.h",
        "%{wks.location}/Source/Editor/**.hpp",
        "%{wks.location}/Source/Editor/**.inl"
    }

    links {
        -- Always link against the engine runtime binaries.
        "SE-Runtime"
    }
-- project "SE-Editor"
