# Copyright (c) 2024-2025 Traian Avram. All rights reserved.
# SPDX-License-Identifier: Apache-2.0.

cmake_minimum_required(VERSION 3.20)
project(ShooterEngine LANGUAGES C CXX)

#==========================================================================================================================================#
#-------------------------------------------------------- BUILD CONFIGURATION TYPES -------------------------------------------------------#
#==========================================================================================================================================#

set(VALID_BUILD_TYPES Debug Development Shipping)
set(CMAKE_BUILD_TYPE "Development" CACHE STRING "Choose build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${VALID_BUILD_TYPES})

# Validate provided build type.
if (NOT CMAKE_BUILD_TYPE IN_LIST VALID_BUILD_TYPES)
    message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}. Valid options are: ${VALID_BUILD_TYPES}")
endif ()

message(STATUS "Configuring build type: ${CMAKE_BUILD_TYPE}")

# NOTE: Don't allow multi-configuration generators to actually have multiple configurations at the same time.
#       This simplifies the build system since there are no differences between generators.
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

#==========================================================================================================================================#
#------------------------------------------------------------ OUTPUT DIRECTORY ------------------------------------------------------------#
#==========================================================================================================================================#

# All generated binaries should be stored directly under the CMake binary directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# NOTE: For multi-configuration generators (such as Visual Studio or XCode) there are separate output directories for each configuration.
#       Since we only have one configuration per project file there is no need for multiple directories.
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR})

#==========================================================================================================================================#
#------------------------------------------------------------ COMPILATION FLAGS -----------------------------------------------------------#
#==========================================================================================================================================#

# Compile using the C++20 standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -pedantic")
        add_compile_definitions("SE_CONFIGURATION_DEBUG=1")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Development")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -Wall")
        add_compile_definitions("SE_CONFIGURATION_DEVELOPMENT=1")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Shipping")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -Wall")
        add_compile_definitions("SE_CONFIGURATION_SHIPPING=1")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi /W3 /EHsc")
        add_compile_definitions("SE_CONFIGURATION_DEBUG=1")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Development")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Zi /W3 /EHsc")
        add_compile_definitions("SE_CONFIGURATION_DEVELOPMENT=1")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Shipping")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /DNDEBUG /W3 /EHsc")
        add_compile_definitions("SE_CONFIGURATION_SHIPPING=1")
    endif ()
endif ()

#==========================================================================================================================================#
#------------------------------------------------------------- RUNTIME TARGETS ------------------------------------------------------------#
#==========================================================================================================================================#

set(SE_RUNTIME_SOURCE_FILES
    Source/Runtime/Application/Input.h
    Source/Runtime/Application/Platform/Windows/WindowsInput.cpp
    Source/Runtime/Application/Platform/Windows/WindowsWindow.cpp
    Source/Runtime/Application/Platform/Windows/WindowsWindow.h
    Source/Runtime/Application/Window.cpp
    Source/Runtime/Application/Window.h

    Source/Runtime/Core/Containers/Allocator.h
    Source/Runtime/Core/Containers/HashMap.h
    Source/Runtime/Core/Containers/HashSet.h
    Source/Runtime/Core/Containers/LockPtr.h
    Source/Runtime/Core/Containers/Optional.h
    Source/Runtime/Core/Containers/OwnPtr.h
    Source/Runtime/Core/Containers/RefPtr.h
    Source/Runtime/Core/Containers/String/String.h
    Source/Runtime/Core/Containers/String/StringView.cpp
    Source/Runtime/Core/Containers/String/StringView.h
    Source/Runtime/Core/Containers/Vector.h
    Source/Runtime/Core/Containers/VectorView.h
    Source/Runtime/Core/Badge.h
    Source/Runtime/Core/CoreAssertions.cpp
    Source/Runtime/Core/CoreAssertions.h
    Source/Runtime/Core/CoreDefines.h
    Source/Runtime/Core/CoreTypes.h
    Source/Runtime/Core/Delegate.cpp
    Source/Runtime/Core/Delegate.h
    Source/Runtime/Core/Hash/CityHash/city.cc
    Source/Runtime/Core/Hash/CityHash/city.h
    Source/Runtime/Core/Hash/CityHash/citycrc.h
    Source/Runtime/Core/Log.cpp
    Source/Runtime/Core/Log.h
    Source/Runtime/Core/Math/Math.cpp
    Source/Runtime/Core/Math/Math.h
    Source/Runtime/Core/Math/Matrix.h
    Source/Runtime/Core/Math/MatrixTransformations.h
    Source/Runtime/Core/Math/Random.cpp
    Source/Runtime/Core/Math/Random.h
    Source/Runtime/Core/Math/Vector.h
    Source/Runtime/Core/Memory/Buffer.cpp
    Source/Runtime/Core/Memory/Buffer.h
    Source/Runtime/Core/Memory/BufferView.h
    Source/Runtime/Core/Memory/MemoryOperations.cpp
    Source/Runtime/Core/Memory/MemoryOperations.h
    Source/Runtime/Core/NumericLimits.h
    Source/Runtime/Core/Platform/PlatformCoreInclude.h
    Source/Runtime/Core/Platform/PlatformTime.h
    Source/Runtime/Core/Platform/Windows/WindowsInclude.h
    Source/Runtime/Core/Platform/Windows/WindowsPlatformTime.cpp
    Source/Runtime/Core/Time.cpp
    Source/Runtime/Core/Time.h
    Source/Runtime/Core/UUID.cpp
    Source/Runtime/Core/UUID.h

    Source/Runtime/Engine/Engine.cpp
    Source/Runtime/Engine/Engine.h
    Source/Runtime/Engine/GameEngine.cpp
    Source/Runtime/Engine/GameEngine.h

    Source/Runtime/Renderer/RHI/Buffer.h
    Source/Runtime/Renderer/RHI/CommandList.h
    Source/Runtime/Renderer/RHI/GraphicsState.h
    Source/Runtime/Renderer/RHI/RenderingDriver.cpp
    Source/Runtime/Renderer/RHI/RenderingDriver.h
    Source/Runtime/Renderer/RHI/RenderingSurface.h
    Source/Runtime/Renderer/RHI/RenderPass.h
    Source/Runtime/Renderer/RHI/RHICore.h
    Source/Runtime/Renderer/RHI/RHIObject.cpp
    Source/Runtime/Renderer/RHI/RHIObject.h
    Source/Runtime/Renderer/RHI/Shader.h
    Source/Runtime/Renderer/RHI/ShaderResource.h
    Source/Runtime/Renderer/RHI/Synchronization.h
    Source/Runtime/Renderer/RHI/Texture.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanBuffer.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanBuffer.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanCommandList.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanCommandList.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanCommandPool.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanCommandPool.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanCore.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSet.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSet.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSetManager.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanDescriptorSetManager.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanFramebuffer.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanFramebuffer.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanMemory.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanMemory.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanPipeline.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanPipeline.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanRenderingSurface.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanRenderPass.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanShader.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanShader.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanSwapchain.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanSwapchain.h
    Source/Runtime/Renderer/RHI/Vulkan/VulkanTexture.cpp
    Source/Runtime/Renderer/RHI/Vulkan/VulkanTexture.h
    Source/Runtime/Renderer/ShaderCompiler/ShaderCompiler.cpp
    Source/Runtime/Renderer/ShaderCompiler/ShaderCompiler.h
    Source/Runtime/Renderer/ShaderCompiler/ShaderReflectionData.h
    Source/Runtime/Renderer/ShaderCompiler/ShaderStage.h
    Source/Runtime/Renderer/ShaderCompiler/spirv.h
    Source/Runtime/Renderer/ShaderCompiler/spirv_reflect.cpp
    Source/Runtime/Renderer/ShaderCompiler/spirv_reflect.h
    Source/Runtime/Renderer/ShaderLibrary.cpp
    Source/Runtime/Renderer/ShaderLibrary.h

    Source/Runtime/Scene/Components/SpriteRendererComponent.h
    Source/Runtime/Scene/Components/TransformComponent.h
    Source/Runtime/Scene/Entity.cpp
    Source/Runtime/Scene/Entity.h
    Source/Runtime/Scene/Forward.h
    Source/Runtime/Scene/Reflection/Fields/Field.h
    Source/Runtime/Scene/Reflection/Fields/FieldType.h
    Source/Runtime/Scene/Reflection/Fields/FieldTypeFinder.h
    Source/Runtime/Scene/Reflection/PrimitiveDataType.h
    Source/Runtime/Scene/Reflection/ReflectionComponent.cpp
    Source/Runtime/Scene/Reflection/ReflectionComponent.h
    Source/Runtime/Scene/Reflection/ReflectionEnum.cpp
    Source/Runtime/Scene/Reflection/ReflectionEnum.h
    Source/Runtime/Scene/Reflection/ReflectionScript.cpp
    Source/Runtime/Scene/Reflection/ReflectionScript.h
    Source/Runtime/Scene/Reflection/ReflectionStruct.cpp
    Source/Runtime/Scene/Reflection/ReflectionStruct.h
    Source/Runtime/Scene/Reflection/ReflectionSystem.cpp
    Source/Runtime/Scene/Reflection/ReflectionSystem.h
    Source/Runtime/Scene/Reflection/SceneReflectionRegistry.cpp
    Source/Runtime/Scene/Reflection/SceneReflectionRegistry.h
    Source/Runtime/Scene/Scene.cpp
    Source/Runtime/Scene/Scene.h
    Source/Runtime/Scene/Script.cpp
    Source/Runtime/Scene/Script.h
    Source/Runtime/Scene/System.cpp
    Source/Runtime/Scene/System.h
)

set(VULKAN_SDK $ENV{VULKAN_SDK})
if (NOT VULKAN_SDK)
    message(FATAL_ERROR "VULKAN_SDK environment variable not set! Please install the Vulkan SDK and make sure it's in your environment.")
endif ()

# Static library.
add_library(SE-Runtime-Static STATIC ${SE_RUNTIME_SOURCE_FILES})
target_include_directories(SE-Runtime-Static PUBLIC ${CMAKE_SOURCE_DIR}/Source)
target_compile_definitions(SE-Runtime-Static PRIVATE "SE_TARGET_RUNTIME_STATIC=1")
# Link against the Vulkan SDK.
target_link_directories(SE-Runtime-Static PUBLIC ${VULKAN_SDK}/Lib)
target_link_libraries(SE-Runtime-Static PUBLIC "vulkan-1" "dxcompiler")
target_include_directories(SE-Runtime-Static PUBLIC ${VULKAN_SDK}/Include)

# Shared library.
add_library(SE-Runtime-Shared SHARED ${SE_RUNTIME_SOURCE_FILES})
target_include_directories(SE-Runtime-Shared PUBLIC ${CMAKE_SOURCE_DIR}/Source)
target_compile_definitions(SE-Runtime-Shared PRIVATE "SE_TARGET_RUNTIME_SHARED=1")
# Link against the Vulkan SDK.
target_link_directories(SE-Runtime-Shared PUBLIC ${VULKAN_SDK}/Lib)
target_link_libraries(SE-Runtime-Shared PUBLIC "vulkan-1" "dxcompiler")
target_include_directories(SE-Runtime-Shared PUBLIC ${VULKAN_SDK}/Include)

#==========================================================================================================================================#
#-------------------------------------------------------------- GAME TARGETS --------------------------------------------------------------#
#==========================================================================================================================================#

set(SE_GAME_SOURCE_FILES
    Source/Game/ShooterGame.cpp
)

# Standalone.
add_executable(SE-Game-Standalone ${SE_GAME_SOURCE_FILES})
target_include_directories(SE-Game-Standalone PUBLIC ${CMAKE_SOURCE_DIR}/Source)
target_compile_definitions(SE-Game-Standalone PRIVATE "SE_TARGET_GAME_STANDALONE=1")
target_link_libraries(SE-Game-Standalone PUBLIC SE-Runtime-Static)
add_dependencies(SE-Game-Standalone SE-Runtime-Static)

# PIE.
add_library(SE-Game-PIE SHARED ${SE_GAME_SOURCE_FILES})
target_include_directories(SE-Game-PIE PUBLIC ${CMAKE_SOURCE_DIR}/Source)
target_compile_definitions(SE-Game-PIE PRIVATE "SE_TARGET_GAME_PIE=1")
target_link_libraries(SE-Game-PIE PUBLIC SE-Runtime-Shared)
add_dependencies(SE-Game-PIE SE-Runtime-Shared)

#==========================================================================================================================================#
#-------------------------------------------------------------- EDITOR TARGET -------------------------------------------------------------#
#==========================================================================================================================================#

set(SE_EDITOR_SOURCE_FILES
    Source/Editor/EditorEngine/EditorEngine.cpp
    Source/Editor/EditorEngine/EditorEngine.h
    Source/Editor/EditorEngine/ShooterEditor.cpp
)

add_executable(SE-Editor ${SE_EDITOR_SOURCE_FILES})
target_include_directories(SE-Editor PUBLIC ${CMAKE_SOURCE_DIR}/Source)
target_compile_definitions(SE-Editor PRIVATE "SE_TARGET_EDITOR=1")
target_link_libraries(SE-Editor PUBLIC SE-Runtime-Shared)
add_dependencies(SE-Editor SE-Runtime-Shared)
