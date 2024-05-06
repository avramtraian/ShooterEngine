--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2.0.
--

project "SE-Editor"
    language "C++"
    cppdialect "C++20"
    
    rtti "Off"
    exceptionhandling "Off"
    characterset "Unicode"
    flags { "MultiProcessorCompile" }

    targetdir (engine_root.."/Binaries/%{cfg.buildcfg}")
    objdir (engine_root.."/Intermediate")
    targetname "SE-Editor"

    staticruntime "Off"
    location (engine_root.."/Source/Editor")
    debugdir (engine_root.."/Binaries/%{cfg.buildcfg}")

    pchheader ("EditorPCH.h")
    pchsource ("../../Source/Editor/EditorPCH.cpp")
    forceincludes { "EditorPCH.h" }

    files {
        "%{prj.location}/**.cpp",
        "%{prj.location}/**.h",
        "%{prj.location}/**.inl"
    }

    includedirs {
        ("%{prj.location}"),
        (engine_root.."/Source/Runtime")
    }

    links {
        "SE-Engine"
    }

    filter "platforms:Windows"
        systemversion "latest"
        defines { "SE_PLATFORM_WINDOWS=1" }
    filter {}

    filter "configurations:Debug"
        kind "ConsoleApp"
        optimize "off"
        symbols "on"
        defines { "SE_CONFIGURATION_DEBUG=1" }
    filter {}

    filter "configurations:Development"
        kind "ConsoleApp"
        optimize "on"
        symbols "on"
        defines { "SE_CONFIGURATION_DEVELOPMENT=1" }
    filter {}

    filter "configurations:Shipping"
        kind "None"
    filter {}