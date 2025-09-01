-- Copyright (c) 2024-2025 Traian Avram. All rights reserved.

project "SE-Runtime"
    location "%{wks.location}/Source/Runtime"
    default_project_configuration()
    defines { "SE_PROJECT_RUNTIME" }

    begin_filter_target_editor()
        -- Build the runtime binaries as a dynamic library when in an editor configuration.
        -- In order to allow game binaries hot-reloading, the gameplay binaries must be compiled
        -- as a dynamic library, which requires to link against the runtime library. But the
        -- editor also requires to link against the runtime, so it must be compiled as a dynamic library.
        kind "SharedLib"
    end_filter()
    
    begin_filter_target_game()
        -- Unlike the Editor build configurations, only the game executable itself links against the runtime
        -- library, and thus it can be compiled as a static library.
        kind "StaticLib"
    end_filter()

    files {
        "%{wks.location}/Source/Runtime/**.cpp",
        "%{wks.location}/Source/Runtime/**.cc",
        "%{wks.location}/Source/Runtime/**.cxx",
        "%{wks.location}/Source/Runtime/**.h",
        "%{wks.location}/Source/Runtime/**.hpp",
        "%{wks.location}/Source/Runtime/**.inl"
    }

    -- Link against the VulkanSDK.
    local VULKAN_PATH = os.getenv("VULKAN_SDK")
    links { (VULKAN_PATH.."/Lib/vulkan-1") }
    includedirs { (VULKAN_PATH.."/Include/") }

    -- Link against the DXC compiler binaries. This allows us to programatically compile HLSL shaders.
    begin_filter_configuration_debug()
        links { (VULKAN_PATH.."/Lib/dxcompilerd") }
    end_filter()
    begin_filter_configuration_development_or_shipping()
        links { (VULKAN_PATH.."/Lib/dxcompiler") }
    end_filter()

-- project "SE-Runtime"
