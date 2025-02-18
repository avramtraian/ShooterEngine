:: Copyright (c) 2024-2025 Traian Avram. All rights reserved.

@echo off
pushd "%~dp0"

set PREMAKE_BUILD_SCRIPT="Content/BuildScripts/Premake-Workspace.lua"
set PREMAKE_TARGET=vs2022
set PREMAKE_EXECUTABLE="Binaries/ThirdParty/Premake/Win64/premake5.exe"

:: Invoke the premake executable in order to generate the project files specified
:: by the provided root Lua build script.
call %PREMAKE_EXECUTABLE% --file=%PREMAKE_BUILD_SCRIPT% %PREMAKE_TARGET%

popd
pause
