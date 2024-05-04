::
:: Copyright (c) 2024 Traian Avram. All rights reserved.
:: SPDX-License-Identifier: Apache-2.0.
::

@echo off
pushd "%~dp0"

call "Binaries/ThirdParty/Premake/Win64/premake5.exe" --file="Content/BuildScripts/EngineWorkspace.lua" vs2022

popd
pause
