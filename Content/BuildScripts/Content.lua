--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2.0.
--

project "SE-Content"
    location (engine_root.."/Intermediate/ProjectFiles")
    kind "None"

    files {
        -- Build scripts.
        (engine_root.."/Content/BuildScripts/**.lua"),

        -- Runtime and editor shaders.
        (engine_root.."/Content/Runtime/Shaders/**.hlsl"),
        (engine_root.."/Content/Editor/Shaders/**.hlsl")
    }