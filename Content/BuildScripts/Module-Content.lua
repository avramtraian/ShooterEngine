--
-- Copyright (c) 2024 Traian Avram. All rights reserved.
-- SPDX-License-Identifier: Apache-2-0.
--

project "SE-Content"
    location "%{wks.location}/Intermediate/ProjectFiles"
    kind "None"

    files
    {
        -- Build scripts.
        "%{wks.location}/Content/BuildScripts/**.lua",

        -- Runtime and editor shaders.
        "%{wks.location}/Content/Runtime/Shaders/**.hlsl",
        "%{wks.location}/Content/Editor/Shaders/**.hlsl"
    }
-- endproject "SE-Content"
