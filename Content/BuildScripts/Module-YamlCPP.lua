--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

project "SE-YamlCPP"
    location "%{wks.location}/Intermediate/ProjectFiles"
    kind "StaticLib"

    configure_default_settings()
    rtti "On"
    exceptionhandling "On"

    files
    {
        "%{wks.location}/Source/ThirdParty/yaml-cpp/**.cpp",
        "%{wks.location}/Source/ThirdParty/yaml-cpp/**.h"
    }

    includedirs
    {
        "%{wks.location}/Source/ThirdParty/yaml-cpp/include",
        "%{wks.location}/Source/ThirdParty/yaml-cpp/src"
    }

    defines
    {
        "YAML_CPP_STATIC_DEFINE"
    }
-- endproject "SE-YamlCPP"
