--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

project "SE-ImGui"
    location "%{wks.location}/Intermediate/ProjectFiles"
    kind "StaticLib"
    configure_default_settings()

    files
    {
        "%{wks.location}/Source/ThirdParty/ImGui/*.cpp",
        "%{wks.location}/Source/ThirdParty/ImGui/*.h"
    }

    includedirs
    {
        "%{wks.location}/Source/ThirdParty/ImGui"
    }

    filter "platforms:windows"
        files
        {
            "%{wks.location}/Source/ThirdParty/ImGui/backends/imgui_impl_dx11.cpp",
            "%{wks.location}/Source/ThirdParty/ImGui/backends/imgui_impl_dx11.h",
            "%{wks.location}/Source/ThirdParty/ImGui/backends/imgui_impl_win32.cpp",
            "%{wks.location}/Source/ThirdParty/ImGui/backends/imgui_impl_win32.h"
        }
    filter {}
-- endproject "SE-ImGui"
