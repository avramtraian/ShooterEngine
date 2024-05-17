--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2.0.
--

project "SE-YamlCPP"
    location (engine_root.."/Intermediate/ProjectFiles")
    language "C++"
    cppdialect "C++20"

    kind "StaticLib"
    staticruntime "Off"

    characterset "Unicode"
    flags { "MultiProcessorCompile" }

    targetdir (engine_root.."/Binaries/%{cfg.buildcfg}")
    objdir (engine_root.."/Intermediate")

    files
    {
        (engine_root.."/Source/ThirdParty/yaml-cpp/**.cpp"),
        (engine_root.."/Source/ThirdParty/yaml-cpp/**.h")
    }

    includedirs
    {
        (engine_root.."/Source/ThirdParty/yaml-cpp/include"),
        (engine_root.."/Source/ThirdParty/yaml-cpp/src")
    }

    defines
    {
        "YAML_CPP_STATIC_DEFINE"
    }

    filter "configurations:Debug"
        optimize "off"
        symbols "on"
    filter {}

    filter "configurations:Development"
        optimize "on"
        symbols "on"
    filter {}

    filter "configurations:Shipping"
        optimize "speed"
        symbols "off"
    filter {}