--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2.0.
--

engine_root = "%{wks.location}"

workspace "ShooterEngine"
    location "../../"

    configurations {
        "Debug",
        "Development",
        "Shipping"
    }

    platforms {
        "Windows"
    }

    filter "platforms:Windows"
        system "Windows"
        architecture "x64"
    filter {}

    startproject "SE-Editor"

    group "Core"
        include "Core-Engine"
    group "Tools"
        include "Tools-Content"
        include "Tools-Editor"
    group "ThirdParty"
        include "TP-YamlCPP"
    group ""