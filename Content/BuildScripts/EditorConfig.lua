--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

-- Utilities library.
require "SharedUtilities"

config_vars = create_default_config_vars_object()
config_vars.target = targets.editor
config_vars.engine_root = "%{wks.location}"

workspace "ShooterEngine"
    location "../../"

    configurations
    {
        "Debug",
        "Development",
        "Shipping"
    }

    platforms
    {
        "Windows"
    }

    filter "platforms:Windows"
        system "Windows"
        architecture "x64"
    filter {}

    startproject "SE-Editor"

    group "Core"
        include "Module-Engine"
    group "Tools"
        include "Module-Content"
        include "Module-Editor"
    group "ThirdParty"
        include "Module-ImGui"
        include "Module-YamlCPP"
    group ""
-- endworkspace "ShooterEngine"
