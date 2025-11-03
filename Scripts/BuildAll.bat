::
:: Copyright (c) 2024-2025 Traian Avram. All rights reserved.
:: SPDX-License-Identifier: Apache-2.0.
::

@ECHO OFF
PUSHD "%~dp0..\"

where cl >nul 2>&1
IF ERRORLEVEL 1 (
    ECHO Skipping configuration that use the MSVC toolchain...
) ELSE (
    :: MSVC toolchain with Visual Studio as the build generator. Same as compiling from the solutions directly.
    cmake --build Build/Debug-MSVC-VS       --config Debug
    cmake --build Build/Development-MSVC-VS --config Development
    cmake --build Build/Shipping-MSVC-VS    --config Shipping

    :: MSVC toolchain with Ninja as the build generator.
    cmake --build Build/Debug-MSVC-Ninja
    cmake --build Build/Development-MSVC-Ninja
    cmake --build Build/Shipping-MSVC-Ninja
)

:: Clang toolchain with Ninja as the build generator.
cmake --build Build/Debug-Clang-Ninja
cmake --build Build/Development-Clang-Ninja
cmake --build Build/Shipping-Clang-Ninja

POPD
