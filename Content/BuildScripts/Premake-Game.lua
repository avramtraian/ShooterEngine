-- Copyright (c) 2024-2025 Traian Avram. All rights reserved.

project "SE-Game"
    location "%{wks.location}/Source/Game"
    default_project_configuration()
    defines { "SE_PROJECT_GAME" }

    begin_filter_target_editor()
        -- Build the runtime binaries as a dynamic library when in an editor configuration.
        -- In order to allow game binaries hot-reloading, the gameplay binaries must be compiled
        -- as a dynamic library.
        kind "SharedLib"
    end_filter()

    begin_filter_target_game()
        -- Compile the game as a standalone executable, which links against the engine binaries
        -- compiled as a static library.
        kind "ConsoleApp"
    end_filter()

    files {
        "%{wks.location}/Source/Game/**.cpp",
        "%{wks.location}/Source/Game/**.cc",
        "%{wks.location}/Source/Game/**.cxx",
        "%{wks.location}/Source/Game/**.h",
        "%{wks.location}/Source/Game/**.hpp",
        "%{wks.location}/Source/Game/**.inl"
    }

    links {
        -- Always link against the engine runtime binaries.
        "SE-Runtime"
    }
-- project "SE-Game"
