-- Copyright (c) 2024-2025 Traian Avram. All rights reserved.

-- Includes utilities that are used when describing the premake projects.
require "Premake-Include"

workspace "ShooterEngine"
    location "../../"

    configurations {
        -- Editor configurations.
        -- Both the engine and game binaries are compiled as dynamic libraries, that the
        -- editor executable links against. The game can't be played as standalone
        -- in this configuration because there is no game executable generated.
        "EditorDebug",
        "EditorDevelopment",
        
        -- Game configurations.
        -- The engine binaries are compiled as a static library and the game binaries
        -- are compiled as an executable, which links against the engine library. No
        -- editor-related code should be compiled in this configuration, so the editor
        -- can never be attached to the game.
        "GameDebug",
        "GameDevelopment",
        "GameShipping"
    }

    platforms {
        -- Currently, the only supported platform is Windows on x86-64 architecture.
        -- However, as more platforms will be supported, they will be appended to this list.
        "Windows"
    }

    filter "platforms:Windows"
        system "Windows"
        -- Only the x86-64 architecture is currently supported.
        architecture "x86_64"
    filter {}

    -- Because the default build configuration is 'EditorDebug', it is expected that
    -- the editor will be launched in execution when the debugger is started.
    startproject "Shooter-Editor"

    group "Game"
        -- Includes projects that are related to gameplay.
        include "Premake-Game"
    group ""

    group "Engine"
        -- Includes projects that are related to the engine, which means it includes
        -- components that are independent from the game itself and could theoretically
        -- be reused for other projects.
        include "Premake-Editor"
        include "Premake-Runtime"
    group ""
-- workspace "ShooterGame"
