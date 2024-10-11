::
:: Copyright (c) 2024 Traian Avram. All rights reserved.
:: SPDX-License-Identifier: Apache-2.0.
::

@echo off
pushd "%~dp0"

set GAME_NAME=@GAME_NAME@
set ENGINE_ROOT=@ENGINE_ROOT@

set GAME_ROOT_DIRECTORY=%~dp0
set PREMAKE_EXECUTABLE=%ENGINE_ROOT%/Binaries/ThirdParty/Premake/Win64/premake5.exe
set PREMAKE_SCRIPT=%ENGINE_ROOT%/Content/BuildScripts/GameConfig.lua

call "%PREMAKE_EXECUTABLE%" --file="%PREMAKE_SCRIPT%" vs2022
popd
pause
