--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2.0.
--

project "SE-Engine"
    language "C++"
    cppdialect "C++20"
    
    rtti "Off"
    exceptionhandling "Off"
    characterset "Unicode"
    flags { "MultiProcessorCompile" }

    targetdir (engine_root.."/Binaries/%{cfg.buildcfg}")
    objdir (engine_root.."/Intermediate")
    targetname "SE-Engine"

    kind "SharedLib"
    staticruntime "Off"
    location (engine_root.."/Source/Runtime")

    files {
        "%{prj.location}/**.cpp",
        "%{prj.location}/**.h",
        "%{prj.location}/**.inl"
    }

    includedirs {
        "%{prj.location}"
    }

    defines {
        "SE_EXPORT_ENGINE_API"
    }

    filter "platforms:Windows"
        systemversion "latest"
        defines { "SE_PLATFORM_WINDOWS=1" }
    filter {}

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